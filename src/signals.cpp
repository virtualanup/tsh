#include "signals.h"

namespace tsh {

handler_type *Signal(int signum, handler_type *handler) {
    struct sigaction action, old_action;

    action.sa_handler = handler;
    sigemptyset(&action.sa_mask); /* block sigs of type being handled */
    action.sa_flags = SA_RESTART; /* restart syscalls if possible */

    if (sigaction(signum, &action, &old_action) < 0)
        throw "Signal Error";
    return (old_action.sa_handler);
}
} // namespace tsh
