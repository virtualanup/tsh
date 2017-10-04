
#include "jobs.h"
namespace tsh {

Command::Command(std::string cmd) : command(cmd) {}

Job::Job() : is_background(false), str("") {}
Job::~Job() {}

} // namespace tsh
