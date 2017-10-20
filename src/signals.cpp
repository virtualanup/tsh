#include "signals.h"
#include "errors.h"
#include "jobs.h"
#include "tsh.h"

#include <sys/wait.h>

namespace tsh {

// Sets the handler to given function and returns the previous handler
// function
handler_type *Signal(int signum, handler_type *handler) {
    struct sigaction action, old_action;

    action.sa_handler = handler;
    sigemptyset(&action.sa_mask); /* block sigs of type being handled */
    action.sa_flags = SA_RESTART; /* restart syscalls if possible */

    if (sigaction(signum, &action, &old_action) < 0)
        unix_error("Signal Error");
    return (old_action.sa_handler);
}

void install_signals() {
    Signal(SIGINT, sigint_handler);    // ctrl-c
    Signal(SIGTSTP, sigtstp_handler);  // ctrl-z
    Signal(SIGCHLD, sigchild_handler); // Terminated or stopped child
    Signal(SIGQUIT, sigquit_handler);
}

// reset the signal handlers to default
void reset_signals() {
    Signal(SIGINT, SIG_DFL);  // ctrl-c
    Signal(SIGTSTP, SIG_DFL); // ctrl-z
    Signal(SIGCHLD, SIG_DFL); // Terminated or stopped child
    Signal(SIGQUIT, SIG_DFL);

    // terminal will stop child process if they are background process and
    // they try to read user input
    Signal(SIGTTIN, SIG_DFL);
    Signal(SIGTTOU, SIG_DFL);
}

// These handler functions just pass the parameter to the
// shell object
void sigchild_handler(int sig) { getShell().sigchild_handler(sig); }
void sigtstp_handler(int sig) { getShell().sigtstp_handler(sig); }
void sigint_handler(int sig) { getShell().sigint_handler(sig); }
void sigquit_handler(int sig) { getShell().sigquit_handler(sig); }

// signal handlers
void Shell::sigchild_handler(int sig) {
    DEBUG_MSG("SigChild received");
    pid_t p;
    int status;

    while ((p = waitpid(-1, &status, WNOHANG | WUNTRACED)) > 0) {
        // Get job from the process ID
        if (pidmap.find(p) == pidmap.end()) {
            DEBUG_MSG("PID " << p << " not found in dictionary in sigchild");
        } else {
            std::shared_ptr<Job> job = pidmap[p];
            if (WIFSTOPPED(status)) {
                job->state = STATE_STOPPED;
                fg_job = NULL;
                job->print_status();
                DEBUG_MSG("Job " << job->jid << " Stopped");
            } else {
                DEBUG_MSG("Removing " << p << "from pid map");
                pidmap.erase(p);
                // A process has terminated. Update the job list
                job->num_processes -= 1;
                DEBUG_MSG("Remaining processes : " << job->num_processes);
                if (job->num_processes == 0) {

                    bool printstatus = false;
                    if (job->state == STATE_BACKGROUND)
                        printstatus = true;
                    // all the processes of the job terminated. Delete the job
                    job->state = STATE_FINISHED;
                    // remove if it was foreground job
                    delete_job(job->jid);
                    DEBUG_MSG("Removing " << job->jid << "from jobs map");
                    // determine if the last command was successful

                    if (WEXITSTATUS(status) == 0)
                        last_command_success = true;
                    else
                        last_command_success = false;

                    if (printstatus)
                        job->print_status();
                }
            }
        }
    }
}
void Shell::sigtstp_handler(int sig) {}
void Shell::sigint_handler(int sig) {}
void Shell::sigquit_handler(int sig) {}

} // namespace tsh
