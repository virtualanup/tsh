#ifndef TSH_TOKENIZER_H
#define TSH_TOKENIZER_H

#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace tsh {

enum TokenType {
    TOK_STR,
    TOK_PIPE,
    TOK_AMP,
    TOK_SCL, // semicolon
    TOK_EOF,
};

const char *const TokenNames[] = {
    "TOK_STR", "TOK_PIPE", "TOK_AMP", "TOK_SCL", "TOK_EOF",
};

class Token {
public:
    TokenType type;
    std::string text;
    size_t start;
    size_t end;
    std::shared_ptr<std::string> command;

    Token(TokenType token, const std::string &text, size_t start, size_t end,
          std::shared_ptr<std::string> command);

    void print() const;
    virtual ~Token();
};

enum TokenizerState { STATE_NORMAL, STATE_QUOTE, STATE_DB_QUOTE, STATE_ERROR };

class Tokenizer {
public:
    std::shared_ptr<std::string> command;
    size_t inppos;
    TokenizerState state;

public:
    Tokenizer();
    virtual ~Tokenizer();

    void add_string(const std::string &);
    const std::string &get_command() const;
    TokenizerState get_state();
    void reset();
    std::unique_ptr<std::vector<Token>> tokenize() const;
};

} // namespace tsh
#endif // TSH_TOKENIZER_H
