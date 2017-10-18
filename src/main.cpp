// TSH : Tiny Shell
// github.com/virtualanup/tsh
//

#include "tsh.h"
#include <getopt.h>
#include <iostream>
#include <unistd.h>

static struct option long_options[] = {
    {"help", 0, NULL, 'h'},      {"verbose", 0, NULL, 'v'},
    {"noprompt", 0, NULL, 'p'},  {"tokens", 0, NULL, 't'},
    {"parsetree", 0, NULL, 'r'}, {NULL, 0, NULL, 0}};

void help_usage();
int main(int argc, char *argv[]) {
    // parse the command line arguments
    char ch;
    tsh::Shell &shell = tsh::getShell();

    // loop over all of the options
    while ((ch = getopt_long(argc, argv, "hpvtr", long_options, NULL)) != -1) {
        // check to see if a single character or long option came through
        switch (ch) {
        case 'h':
            help_usage();
            break;
        case 'v':
            std::cout << "Verbose mode is on" << std::endl;
            break;
        case 'p':
            shell.set_show_prompt(false);
            break;
        case 't':
            shell.set_print_tokens(true);
            break;
        case 'r':
            shell.set_print_parsetree(true);
            break;
        }
    }

    bool is_tty;

    if (isatty(fileno(stdin))) {
        is_tty = true;
    }

    shell.initialize();
    shell.set_tty(is_tty);
    shell.start();
    return 0;
}

void help_usage() {
    std::cout << "Usage : tsh [Option..]" << std::endl << std::endl;

    std::cout << "Options:" << std::endl << std::endl;

    std::cout << "    -h    --help \t\tPrint this help screen" << std::endl;
    std::cout << "    -v    --verbose \t\tPrint debug information(Verbose mode)"
              << std::endl;
    std::cout << "    -p    --noprompt \t\tDon't emit a command prompt"
              << std::endl;
    std::cout << "    -t    --tokens \t\tPrint Tokens" << std::endl;
    std::cout << "    -r    --parsetree \t\tPrint Parse Tree" << std::endl;

    exit(0);
}
