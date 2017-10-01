#include "tsh.h"

Tsh::Tsh(bool interactive) { cwd = getcwd(NULL, 0); }
Tsh::~Tsh() {}

void Tsh::start() {
    // enter the main loop for the shell
    while (true) {
        std::cout << cwd << std::endl;
        std::cout<<"> ";

        std::getline(std::cin, command);

        // process the command

        if (std::cin.eof()) {
            break;
        }
    }
}
