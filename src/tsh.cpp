#include "tsh.h"

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
    : cwd(""), command(""), prompt_str("▶ "), is_tty(false),
      last_command_success(false), show_prompt(true) {
    std::cout << "Shell created" << std::endl;
}

void Shell::initialize() { cwd = getcwd(NULL, 0); }

void Shell::set_tty(bool tty) { is_tty = tty; }

void Shell::set_show_prompt(bool show) { show_prompt = show; }

void Shell::set_prompt(const std::string &prompt) { prompt_str = prompt + " "; }

void Shell::start() {
    // enter the main loop for the shell
    while (true) {
        // read line via readline
        char *rl_cmd;

        std::string prompt;
        // print the prompt in red color if last command was error. Else,
        // print in white.
        if (is_tty && show_prompt) {
            // print the path in blue color
            prompt += blue + cwd + reset + "\n";
            last_command_success = !last_command_success;

            if (last_command_success)
                prompt += red + prompt_str + reset;
            else
                prompt += prompt_str;
        } else
            prompt = "";

        rl_cmd = readline(prompt.c_str());

        if (!rl_cmd)
            break;

        command = rl_cmd;

        // free the memory
        free(rl_cmd);

        // process the command
        std::cout << command << std::endl;
    }
}
} // namespace tsh
