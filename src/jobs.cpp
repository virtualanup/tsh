
#include "jobs.h"
namespace tsh {

Command::Command(std::string cmd) : command(cmd) {}

bool Command::is_builtin() {
    if (command == "cd" || command == "exit" || command == "fg" ||
        command == "bg")
        return true;
    return false;
}

Job::Job() : is_background(false), str(""), num_processes(0), jid(0) {}
Job::~Job() {}

} // namespace tsh
