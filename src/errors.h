#ifndef TSH_ERRORS_H
#define TSH_ERRORS_H

#include <string>

namespace tsh {

void unix_error(const std::string &);
void tsh_error(const std::string &);

} // namespace tsh
#endif // TSH_ERRORS_H
