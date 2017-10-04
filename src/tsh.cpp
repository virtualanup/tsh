#include "tsh.h"
#include "tokenizer.h"

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
      last_command_success(false), show_prompt(true) {
    std::cout << "Shell created" << std::endl;
}

void Shell::initialize() { cwd = getcwd(NULL, 0); }

void Shell::set_tty(bool tty) { is_tty = tty; }

void Shell::set_show_prompt(bool show) { show_prompt = show; }

void Shell::set_prompt(const std::string &prompt) { prompt_str = prompt + " "; }

void Shell::start() {
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

        // process the command
        std::cout << tokenizer.get_command() << std::endl;

        auto tokens = tokenizer.tokenize();
        for(size_t i=0; i<tokens->size(); i++)
            (*tokens)[i].print();

    }
}
} // namespace tsh
