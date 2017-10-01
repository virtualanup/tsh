#ifndef TSH_H
#define TSH_H

#include <iostream>
#include <istream>
#include <string>
#include <unistd.h>

class Tsh {
protected:
    std::string cwd;
    std::string command;

public:
    Tsh(bool interactive);
    ~Tsh();

    void start();
};

#endif
