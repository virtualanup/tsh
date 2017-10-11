#ifndef TSH_SIGNALS_H
#define TSH_SIGNALS_H

#include <csignal>
namespace tsh {

// signal handler signature
typedef void handler_type(int);

handler_type *Signal(int signum, handler_type *handler);
void install_signals();
void reset_signals();


void sigchild_handler(int sig);
void sigtstp_handler(int sig);
void sigint_handler(int sig);
void sigquit_handler(int sig);

} // namespace tsh
#endif // TSH_SIGNALS_H
