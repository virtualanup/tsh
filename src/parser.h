#ifndef TSH_PARSER_H
#define TSH_PARSER_H

#include "tokenizer.h"
#include <string>
#include <vector>

namespace tsh {

enum JobState {
    STATE_UNDEFINED,
    STATE_FOREGROUND,
    STATE_BACKGROUND,
    STATE_STOPPED,
    STATE_FINISHED
};

// This represent a single command : with parameter list
class Command {
public:
    Command(std::string);
    std::string command;
    std::vector<std::string> arguments;
};

class Job {
public:
    bool is_background;
    JobState state;  // state of the job
    std::string str; // string for the entire job
    // commands for the job
    std::vector<std::shared_ptr<Command>> commands;
    Job();
    virtual ~Job();
};

class Parser {
public:
    Parser();
    virtual ~Parser();
    std::shared_ptr<std::vector<std::shared_ptr<Job>>> parse(const Tokenizer &);
};

} // namespace tsh
#endif // TSH_PARSER_H
