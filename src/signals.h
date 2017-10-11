#ifndef TSH_SIGNALS_H
#define TSH_SIGNALS_H

#include <csignal>
namespace tsh {
typedef void handler_type(int);

handler_type *Signal(int signum, handler_type *handler);
} // namespace tsh
#endif // TSH_SIGNALS_H
