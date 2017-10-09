#ifndef TSH_H
#define TSH_H

#include <iostream>
#include <istream>
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

class Shell {
protected:
    std::string cwd;
    std::string prompt_str;
    // Prompt to show when more input is required
    std::string partial_prompt_str;

    bool is_tty;
    bool last_command_success;
    bool show_prompt;
    bool print_tokens;
    bool print_parse_tree;

    Shell();
    virtual ~Shell() {}

    void run_builtin_command(std::shared_ptr<Job>);

public:
    void set_tty(bool tty);
    void set_show_prompt(bool show);
    void set_print_tokens(bool print);
    void set_print_parsetree(bool print);

    void set_prompt(const std::string &);
    void initialize();
    void start();

    int runjob(std::shared_ptr<Job>);

    bool is_builtin(const std::string &);

    friend Shell &getShell();
};

} // namespace tsh
#endif
