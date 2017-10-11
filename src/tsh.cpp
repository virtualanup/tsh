#include "tsh.h"
#include "parser.h"
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
      print_parse_tree(false), fg_process(NULL), max_jid(0)

{}

void Shell::initialize() { cwd = getcwd(NULL, 0); }

void Shell::set_tty(bool tty) { is_tty = tty; }

void Shell::set_show_prompt(bool show) { show_prompt = show; }
void Shell::set_print_tokens(bool print) { print_tokens = print; }
void Shell::set_print_parsetree(bool print) { print_parse_tree = print; }

void Shell::set_prompt(const std::string &prompt) { prompt_str = prompt + " "; }

void Shell::start() {
    Parser parser;
    Tokenizer tokenizer;
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

        if (!rl_cmd)
            break;

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

        // debug information about the tokens
        if (print_tokens) {
            auto tokens = tokenizer.tokenize();
            std::cout << "Tokens:" << std::endl;
            for (size_t i = 0; i < tokens->size(); i++) {
                (*tokens)[i].print();
            }
            std::cout << "End of tokens" << std::endl;
        }

        // parse the command

        try {
            auto job_list = parser.parse(tokenizer);
            if (print_parse_tree) {
                std::cout << std::endl << std::endl;
                std::cout << "Parse tree" << std::endl;
                for (auto job = job_list->begin(); job != job_list->end();
                     job++) {
                    std::cout << std::endl;
                    std::cout << "'" << (*job)->str << "'" << std::endl;
                    if ((*job)->is_background)
                        std::cout << "Background job" << std::endl;
                    else
                        std::cout << "Foreground job" << std::endl;
                    std::cout << (*job)->commands.size()
                              << " piped commands in job" << std::endl;
                    for (auto cmd = (*job)->commands.begin();
                         cmd != (*job)->commands.end(); cmd++) {
                        std::cout << "\t" << '"' << (*cmd).command << '"'
                                  << std::endl;
                        // print the arguments
                        for (auto arg = cmd->arguments.begin();
                             arg != cmd->arguments.end(); arg++)
                            std::cout << "\t\t" << '"' << (*arg) << '"'
                                      << std::endl;
                    }
                    std::cout << std::endl;
                }
                // debug information about the jobs
                std::cout << job_list->size() << " job_list in the command"
                          << std::endl;
            }

            // Run the command
            for (auto job = job_list->begin(); job != job_list->end(); job++) {
                if (runjob(*job) == 0)
                    last_command_success = true;
                else
                    last_command_success = false;
            }

        } catch (const std::string &error) {
            std::cout << "Error : " << error << std::endl;
            last_command_success = false;
            continue;
        }
    }
}

int Shell::runjob(std::shared_ptr<Job> job) {
    // Run the job. If the job is foreground process, block until the
    // job is finished or "ctrl-z" is pressed.

    // Count the number of commands we expect
    job->num_processes = job->commands.size();

    // Assign an integer id to the job
    job->jid = get_next_jid();

    // Add the job to our jobs list
    jobs[job->jid] = job;

    if (!job->is_background)
        fg_process = job;

    int input = STDIN_FILENO;
    int output = STDOUT_FILENO;

    for (auto cmd = job->commands.begin(); cmd != job->commands.end(); cmd++) {
        if (cmd->is_builtin()) {
            // TODO: Close stdin
            input = STDIN_FILENO;
            output = STDOUT_FILENO;

            output += input;
            input += output;

            if(!run_builtin(*cmd))
                last_command_success = false;
        }
    }
    return 0;
}

unsigned int Shell::get_next_jid() {
    while (jobs.count(max_jid) == 1)
        max_jid = (max_jid + 1) % USHRT_MAX;
    return max_jid;
}

bool Shell::run_builtin(const Command &cmd) {
    if (cmd.command == "exit") {
        // Get the exit code if provided
        int code = 0;
        if (cmd.arguments.size() > 0)
            code = atoi(cmd.arguments[0].c_str());
        exit(code);

    } else if (cmd.command == "cd") {
        if (cmd.arguments.size() > 0) {
            if (chdir(cmd.arguments[0].c_str()) == -1) {
                // some error occured.
                // print error message
                return false;
            }
            cwd = getcwd(NULL, 0);
        }
    } else if (cmd.command == "fg") {

    } else if (cmd.command == "bg") {
    }
    return true;
}
} // namespace tsh
