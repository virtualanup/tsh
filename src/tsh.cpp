#include "tsh.h"
#include "errors.h"
#include "parser.h"
#include "signals.h"
#include "tokenizer.h"

#include <climits>
#include <readline/history.h>
#include <readline/readline.h>

namespace tsh {

const std::string red("\033[1;31m");
const std::string reset("\033[0m");
const std::string blue("\033[1;34m");

Shell &getShell() {
    static Shell shell;
    return shell;
}

Shell::Shell()
    : cwd(""), prompt_str("▶ "), partial_prompt_str("◀ "), is_tty(false),
      last_command_success(true), show_prompt(true), print_tokens(false),
      print_parse_tree(false), fg_job(NULL), max_jid(0)

{
    install_signals();
}

void Shell::initialize() { cwd = getcwd(NULL, 0); }

void Shell::set_tty(bool tty) {
    DEBUG_MSG("Setting tty to " << tty);
    is_tty = tty;
}

void Shell::set_show_prompt(bool show) {
    DEBUG_MSG("Setting show_prompt to " << show);
    show_prompt = show;
}
void Shell::set_print_tokens(bool print) { print_tokens = print; }
void Shell::set_print_parsetree(bool print) { print_parse_tree = print; }

void Shell::set_prompt(const std::string &prompt) { prompt_str = prompt + " "; }

void Shell::start() {
    Parser parser;
    Tokenizer tokenizer;

    // Ignore print in the main shell when in background mode
    Signal(SIGTTIN, SIG_IGN);
    Signal(SIGTTOU, SIG_IGN);

    if (is_tty) {
        tcsetpgrp(STDIN_FILENO, getpgrp());
        tcsetpgrp(STDOUT_FILENO, getpgrp());
    }

    // enter the main loop for the shell
    while (true) {
        // read line via readline
        char *rl_cmd;
        std::string prompt;

        tokenizer.reset();
        // print the prompt in red color if last command was error. Else,
        // print in white.
        if (is_tty && show_prompt) {
            // print the path in blue color
            prompt += blue + cwd + reset + "\n";
            if (!last_command_success)
                prompt += red + prompt_str + reset;
            else
                prompt += prompt_str;
        } else
            prompt = "";

        rl_cmd = readline(prompt.c_str());

        if (!rl_cmd) {
            break;
        }
        // add to tokenizer
        tokenizer.add_string(rl_cmd);

        // free the memory
        free(rl_cmd);

        while (tokenizer.get_state() == STATE_DB_QUOTE ||
               tokenizer.get_state() == STATE_QUOTE) {

            // Add \n as user can be giving multiline input
            tokenizer.add_string("\n");

            // Incomplete input. Get remainder of input
            std::string nextinput;
            std::string prompt;
            if (is_tty && show_prompt)
                prompt = partial_prompt_str;
            rl_cmd = readline(prompt.c_str());

            // Add rl_cmd to string
            if (!rl_cmd) {
                // ERROR: Incomplete input
                break;
            }
            tokenizer.add_string(rl_cmd);
            free(rl_cmd);
        }

        last_command_success = true;

        // debug information about the tokens
        if (print_tokens) {
            auto tokens = tokenizer.tokenize();
            std::cout << "Tokens:" << std::endl;

            for (const auto &token : *tokens)
                token.print();
            std::cout << "End of tokens" << std::endl;
        }

        // parse the command

        try {
            auto job_list = parser.parse(tokenizer);
            if (print_parse_tree) {
                std::cout << std::endl << std::endl;
                std::cout << "Parse tree" << std::endl;

                for (const auto &job : *job_list) {
                    std::cout << std::endl;
                    std::cout << "'" << job->str << "'" << std::endl;
                    if (job->is_background)
                        std::cout << "Background job" << std::endl;
                    else
                        std::cout << "Foreground job" << std::endl;
                    std::cout << job->commands.size()
                              << " piped commands in job" << std::endl;

                    for (const auto &cmd : job->commands) {
                        std::cout << "\t" << '"' << cmd.command << '"'
                                  << std::endl;
                        // print the arguments
                        for (const auto &arg : cmd.arguments)
                            std::cout << "\t\t" << '"' << arg << '"'
                                      << std::endl;
                    }
                    std::cout << std::endl;
                }
                // debug information about the jobs
                std::cout << job_list->size() << " job_list in the command"
                          << std::endl;
            }

            // Run the command
            for (auto &job : *job_list)
                runjob(job);

        } catch (const std::string &error) {
            std::cout << "Error Exception : " << error << std::endl;
            last_command_success = false;
        }
    }
}

void Shell::runjob(std::shared_ptr<Job> job) {
    // Run the job. If the job is foreground process, block until the
    // job is finished or "ctrl-z" is pressed.

    // Count the number of commands we expect
    job->num_processes = job->commands.size();

    // Assign an integer id to the job
    job->jid = get_next_jid();

    // Add the job to our jobs list
    jobs[job->jid] = job;

    if (!job->is_background) {
        // update the foreground job
        fg_job = job;
        job->state = STATE_FOREGROUND;
    } else
        job->state = STATE_BACKGROUND;

    int input = STDIN_FILENO;
    int output = STDOUT_FILENO;

    // file descriptors for pipes
    int pipefd[2];

    unsigned int index = 0;
    for (auto &cmd : job->commands) {
        index += 1;

        if (cmd.is_builtin()) {
            // Since we don't create any process for this
            job->num_processes -= 1;
            // built-in command
            if (!run_builtin(cmd))
                last_command_success = false;
        } else {
            // Create pipes if the command is not last command
            if (index != job->commands.size()) {
                if (pipe(pipefd) == -1) {
                    unix_error("Error creating Pipes : ");
                    close_descriptor(input);
                    return;
                }
                output = pipefd[1];
                DEBUG_MSG("Pipe created for command " << cmd.command);
            } else {
                output = STDOUT_FILENO;
                DEBUG_MSG("Pipe not created for list command " << cmd.command);
            }

            // fork a child process
            int pid = fork();
            if (pid < 0) {
                tsh_error("Error in fork");
                return;
            } else if (pid > 0) {
                // Parent process
                DEBUG_MSG("Parent process : Child process " << pid
                                                            << " Created");
                // Map the newly created process ID to the job
                pidmap[pid] = job;
                if (job->pgid == 0) {
                    job->pgid = pid;
                    DEBUG_MSG("Initializing the pid of job to " << job->pgid);
                }
                // set group ID of the child process
                if (setpgid(pid, job->pgid) == -1) {
                    unix_error("Error in setting group ID in parent");
                    close_descriptor(input);
                    close_descriptor(pipefd[0]);
                    close_descriptor(pipefd[1]);
                    return;
                }

                // if the job is foreground job, give controlling terminal
                if (!job->is_background) {
                    tcsetpgrp(STDIN_FILENO, job->pgid);
                    DEBUG_MSG("group for process " << pid << " Changed to "
                                                   << job->pgid);
                }
                // close the reading end of previous process if it exist
                close_descriptor(input);
                // close the writing end of the current pipe here
                close_descriptor(output);

                // clean the pipes and set output as input of another process
                input = pipefd[0];
            } else {
                // child process
                DEBUG_MSG("child process with process ID " << getpid()
                                                           << " started");
                reset_signals();
                if (job->pgid == 0) {
                    job->pgid = getpid();
                    DEBUG_MSG("First process....setting job pid to "
                              << job->pgid);
                }
                if (setpgid(0, job->pgid) == -1) {
                    close_descriptor(input);
                    close_descriptor(pipefd[0]);
                    close_descriptor(pipefd[1]);
                    unix_error("Error in setting group ID in Child Process");
                    continue;
                }

                // Set the pipes to input and output
                if (input != STDIN_FILENO) {
                    dup2(input, STDIN_FILENO);
                    close(input);
                }

                if (output != STDOUT_FILENO) {
                    dup2(output, STDOUT_FILENO);
                    close(output);
                }

                // close one end of the pipe
                close_descriptor(pipefd[0]);

                // if the job is not background, give terminal access
                // (controlling terminal)
                if (!job->is_background) {
                    tcsetpgrp(STDIN_FILENO, job->pgid);
                }

                // jobs is an internal command but we fork a child process
                // and allow pipe redirction to it.
                if (cmd.command == "jobs") {
                    list_jobs();
                } else if (tsh_execvp(cmd) < 0) {
                    unix_error(std::string("Error : ") + cmd.command);
                }
                // Command may be successful. Exit
                exit(2);
            }
        }
    }

    if (job->is_background) {
        // print status message
        std::cout << std::endl
                  << "[" << job->jid << "] " << job->pgid << std::endl;
        last_command_success = true;
    } else {
        if (job->num_processes == 0) {
            // all the commands were internal commands.
            delete_job(job->jid);
        } else {
            // Parent must wait for the child process to finish
            waitfg();
        }
    }
    return;
}

unsigned int Shell::get_next_jid() {
    while (jobs.count(max_jid) == 1)
        max_jid = (max_jid + 1) % USHRT_MAX;
    return max_jid;
}

void Shell::waitfg() {
    std::shared_ptr<Job> job = fg_job;
    // wait for the foreground process to terminate
    if (job != NULL) {

        if (is_tty) {
            // set the job group ID as the controlling terminal
            if (tcsetpgrp(STDIN_FILENO, job->pgid) < 0) {
                tsh_error("TRM : Error in tc set pgrp for child\n");
            }
        }

        while (fg_job != NULL)
            ;

        if (is_tty) {
            if (tcsetpgrp(STDIN_FILENO, getpgrp()) < 0) {
                tsh_error("TRM : Error in tc set pgrp for parent\n");
            }
        }
    }
}
bool Shell::run_builtin(const Command &cmd) {
    if (cmd.command == "exit") {
        // Get the exit code if provided
        int code = 0;
        if (cmd.arguments.size() > 0)
            code = atoi(cmd.arguments[0].c_str());
        exit(code);

    } else if (cmd.command == "cd") {
        if (cmd.arguments.size() > 1) {
            if (chdir(cmd.arguments[1].c_str()) == -1) {
                // some error occured.
                // print error message
                unix_error("Error");
                return false;
            }
            cwd = getcwd(NULL, 0);
        }
    } else if (cmd.command == "fg") {
        std::cout << "fg is not implemented yet" << std::endl;
    } else if (cmd.command == "bg") {
        std::cout << "bg is not implemented yet" << std::endl;
    }
    return true;
}

void Shell::list_jobs() const {
    for (const auto &jobpair : jobs) {
        std::cout << "[" << jobpair.first << "]\t"
                  << jobpair.second->get_str_state() << "\t"
                  << jobpair.second->str << std::endl;
    }
    exit(0);
}

int Shell::tsh_execvp(const Command &cmd) {
    std::vector<char *> strcmds;
    for (const auto &strcmd : cmd.arguments) {
        strcmds.push_back(const_cast<char *>(strcmd.c_str()));
    }
    strcmds.push_back(NULL);
    char **command = &strcmds[0];
    return execvp(command[0], command);
}

void Shell::close_descriptor(int desc) {
    if ((desc != STDIN_FILENO) && (desc != STDOUT_FILENO))
        close(desc);
}

void Shell::delete_job(int jid) {
    if (jobs.count(jid) > 0) {
        std::shared_ptr<Job> job = jobs[jid];
        if (job == fg_job)
            fg_job = NULL;
        jobs.erase(jid);
    }
}
} // namespace tsh
