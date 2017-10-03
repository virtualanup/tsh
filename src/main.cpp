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

    tsh::Shell &shell = tsh::getShell();
    shell.initialize();
    shell.set_tty(is_tty);
    shell.start();
    return 0;
}
