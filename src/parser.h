#ifndef TSH_PARSER_H
#define TSH_PARSER_H

#include "tokenizer.h"
#include"jobs.h"
#include <string>
#include <vector>

namespace tsh {

class Parser {
public:
    Parser();
    virtual ~Parser();
    std::shared_ptr<std::vector<std::shared_ptr<Job>>> parse(const Tokenizer &);
};

} // namespace tsh
#endif // TSH_PARSER_H
