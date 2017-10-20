
#include "jobs.h"
namespace tsh {

Command::Command(std::string cmd) : command(cmd) { arguments.push_back(cmd); }

bool Command::is_builtin() {
    if (command == "cd" || command == "exit" || command == "fg" ||
        command == "bg")
        return true;
    return false;
}

Job::Job() : is_background(false), str(""), num_processes(0), jid(0) {}
Job::~Job() {}

std::string Job::get_str_state() const {
    if (state == STATE_STOPPED)
        return std::string("Stopped");
    if (state == STATE_FINISHED)
        return std::string("Done");
    if (state == STATE_UNDEFINED)
        return std::string("Undefined");
    return std::string("Running");
}

void Job::print_status() const {
    std::cout << std::endl
              << "[" << jid << "] " << pgid << " " << get_str_state() << "\t"
              << str << std::endl;
}

} // namespace tsh
