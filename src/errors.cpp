#include "errors.h"
#include <cerrno>
#include <cstring>
#include <iostream>

namespace tsh {

// Some system error occured. Get the string message and print it
void unix_error(const std::string &msg) {
    std::cout << msg << ": " << std::strerror(errno) << std::endl;
    // Exit?
}

// Shell error. Just print the message
void tsh_error(const std::string &msg) { std::cout << msg << std::endl; }

} // namespace tsh
