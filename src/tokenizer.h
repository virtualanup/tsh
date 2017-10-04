#ifndef TSH_TOKENS_H
#define TSH_TOKENS_H

#include <string>

namespace tsh {

enum TokenType {
    TOK_STR,
    TOK_PIPE,
    TOK_AMP,
    TOK_EOF,
};

struct Token {
    std::string text;
    TokenType type;
};

enum TokenizerState { STATE_NORMAL, STATE_QUOTE, STATE_DB_QUOTE, STATE_ERROR };

class Tokenizer {
    std::string command;
    unsigned int pos;
    TokenizerState state;

public:
    Tokenizer();
    virtual ~Tokenizer();
    TokenizerState get_state();
    void reset();
    void add_string(const std::string &);
    const std::string& get_command();
};

} // namespace tsh
#endif // TSH_TOKENS_H
