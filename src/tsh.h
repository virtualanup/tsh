#ifndef TSH_H
#define TSH_H

#include "shared.h"

#include <iostream>
#include <istream>
#include <map>
#include <memory>
#include <string>
#include <unistd.h>
namespace tsh {

extern const std::string red;
extern const std::string reset;
extern const std::string blue;

// Singeleton design pattern is used as there can only be one
// instance of shell. The shell object can only be accessed
// via getShell.

class Shell;
Shell &getShell();

class Job;
class Command;

class Shell {
protected:
    std::string history_file;
    std::string cwd;
    std::string home_dir;
    std::string prompt_str;
    // Prompt to show when more input is required
    std::string partial_prompt_str;

    bool is_tty; // flag to check if it is running in TTY mode
    bool
        last_command_success; // flag to check if the last command was a success
    bool show_prompt;         // show prompt or not
    bool print_tokens;
    bool print_parse_tree;
    bool keep_history;

    // The job that is currently in the foreground (with the controlling
    // terminal)
    std::shared_ptr<Job> fg_job;

    // This is used to store the currently running jobs. They are integer
    // indexed
    std::map<unsigned int, std::shared_ptr<Job>> jobs;

    std::map<unsigned int, std::shared_ptr<Job>> pidmap;

    // The maximum value jid currently in use
    unsigned int max_jid;

    // constructor and destructor are protected for singeleton pattern
    Shell();
    virtual ~Shell() {}

    unsigned int get_next_jid();
    bool run_builtin(const Command &cmd);

    void list_jobs() const;
    int tsh_execvp(const Command &);

    void close_descriptor(int);
    void waitfg();

    void delete_job(int jid);
    void update_cwd();

    void add_command_history(const std::string&);

public:
    void set_tty(bool tty);
    void set_show_prompt(bool show);
    void set_print_tokens(bool print);
    void set_print_parsetree(bool print);

    void set_prompt(const std::string &);
    void initialize();
    void start();

    void runjob(std::shared_ptr<Job>);

    // signal handlers
    void sigchild_handler(int sig);
    void sigtstp_handler(int sig);
    void sigint_handler(int sig);
    void sigquit_handler(int sig);

    friend Shell &getShell();
};

} // namespace tsh
#endif
