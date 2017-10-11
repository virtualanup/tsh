#include "signals.h"
#include "tsh.h"

namespace tsh {

// Sets the handler to given function and returns the previous handler
// function
handler_type *Signal(int signum, handler_type *handler) {
    struct sigaction action, old_action;

    action.sa_handler = handler;
    sigemptyset(&action.sa_mask); /* block sigs of type being handled */
    action.sa_flags = SA_RESTART; /* restart syscalls if possible */

    if (sigaction(signum, &action, &old_action) < 0)
        throw "Signal Error";
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

} // namespace tsh
