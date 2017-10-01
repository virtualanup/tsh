// TSH : Tiny Shell
// github.com/virtualanup/tsh
//

#include "tsh.h"
#include <iostream>
#include<unistd.h>

int main(int argc, char *argv[]) {
    bool is_tty;

    if (isatty(fileno(stdin))) {
        is_tty = true;
    }

    Tsh tsh(is_tty);
    tsh.start();

    return 0;
}
