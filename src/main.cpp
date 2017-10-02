// TSH : Tiny Shell
// github.com/virtualanup/tsh
//

#include "tsh.h"
#include <iostream>
#include <unistd.h>

int main(int argc, char *argv[]) {
    bool is_tty;

    if (isatty(fileno(stdin))) {
        is_tty = true;
    }

    tsh::Shell::initialize();
    tsh::Shell::set_tty(is_tty);
    tsh::Shell::start();

    return 0;
}
