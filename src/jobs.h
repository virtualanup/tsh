#ifndef TSH_JOBS_H
#define TSH_JOBS_H

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
    std::string command;
    std::vector<std::string> arguments;

    Command(std::string);
    bool is_builtin();
};

class Job {
public:
    bool is_background;
    JobState state;    // state of the job
    std::string str;   // string for the entire job

    int num_processes; // Number of running processes in the current job

    int pgid;

    unsigned int jid;  // Job ID

    // commands for the job
    std::vector<Command> commands;

    std::string get_str_state() const;
    Job();
    virtual ~Job();
};

} // namespace tsh
#endif // TSH_JOBS_H
