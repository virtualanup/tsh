#ifndef TSH_H
#define TSH_H

#include <iostream>
#include <istream>
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

class Shell {
protected:
    std::string cwd;
    std::string prompt_str;
    // Prompt to show when more input is required
    std::string partial_prompt_str;

    bool is_tty;
    bool last_command_success;
    bool show_prompt;

private:
    Shell();
    virtual ~Shell() {}

public:
    void set_tty(bool tty);
    void set_show_prompt(bool show);
    void set_prompt(const std::string &);
    void initialize();
    void start();

    friend Shell &getShell();
};

} // namespace tsh
#endif
