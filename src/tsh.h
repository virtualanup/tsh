#ifndef TSH_H
#define TSH_H

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
    std::string cwd;
    std::string prompt_str;
    // Prompt to show when more input is required
    std::string partial_prompt_str;

    bool is_tty; // flag to check if it is running in TTY mode
    bool
        last_command_success; // flag to check if the last command was a success
    bool show_prompt;         // show prompt or not
    bool print_tokens;
    bool print_parse_tree;

    // The job that is currently in the foreground (with the controlling terminal)
    std::shared_ptr<Job> fg_process;

    // This is used to store the currently running jobs. They are integer indexed
    std::map<unsigned int, std::shared_ptr<Job>> jobs;

    // The maximum value jid currently in use
    unsigned int max_jid;

    Shell();
    virtual ~Shell() {}

    void run_builtin_command(std::shared_ptr<Job>);
    unsigned int get_next_jid();

public:
    void set_tty(bool tty);
    void set_show_prompt(bool show);
    void set_print_tokens(bool print);
    void set_print_parsetree(bool print);

    void set_prompt(const std::string &);
    void initialize();
    void start();

    int runjob(std::shared_ptr<Job>);

    bool run_builtin(const Command&);

    friend Shell &getShell();
};

} // namespace tsh
#endif
