#include "tsh.h"

namespace tsh {

const std::string red("\033[1;31m");
const std::string reset("\033[0m");
const std::string blue("\033[1;34m");

// Initialize the static members
std::string Shell::cwd("");
std::string Shell::command("");
std::string Shell::prompt_str("▶ ");

bool Shell::is_tty = false;
bool Shell::last_command_success = true;
bool Shell::show_prompt = true;

void Shell::initialize() { cwd = getcwd(NULL, 0); }

void Shell::set_tty(bool tty) { is_tty = tty; }

void Shell::set_show_prompt(bool show) { show_prompt = show; }

void Shell::set_prompt(const std::string &prompt) { prompt_str = prompt + " "; }

void Shell::start() {
    // enter the main loop for the shell
    while (true) {
        if (is_tty && show_prompt) {
            // print the path in blue color
            std::cout << std::endl << blue << cwd << reset << std::endl;
            // print the prompt in red color if last command was error. Else,
            // print in white.
            if (last_command_success)
                std::cout << prompt_str;
            else
                std::cout << red << prompt_str << reset;
            last_command_success = !last_command_success;
        }

        std::getline(std::cin, command);

        // process the command

        if (std::cin.eof()) {
            break;
        }
    }
}
} // namespace tsh
