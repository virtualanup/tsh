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

class Shell {
protected:
    static std::string cwd;
    static std::string command;
    static std::string prompt_str;
    static bool is_tty;
    static bool last_command_success;
    static bool show_prompt;

private:
    Shell() {}
    virtual ~Shell() {}

public:
    static void set_tty(bool tty);
    static void set_show_prompt(bool show);
    static void set_prompt(const std::string&);
    static void initialize();
    static void start();
};

} // namespace tsh
#endif
