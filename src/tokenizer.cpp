#include "tokenizer.h"
namespace tsh {

Tokenizer::Tokenizer() { reset(); }
Tokenizer::~Tokenizer() {}

TokenizerState Tokenizer::get_state() { return state; }

void Tokenizer::reset() {
    state = STATE_NORMAL;
    pos = 0;
    command = "";
}

void Tokenizer::add_string(const std::string &str) {
    // Add a string to the tokenizer. This will update the state of the
    // tokenizer so that if current string ends in open quote, we ask the
    // user for more input

    command += str;

    while (pos < command.size()) {
        if (pos == 0 || command[pos - 1] != '\\') {
            if (command[pos] == '"') {
                if (state == STATE_NORMAL)
                    state = STATE_DB_QUOTE;
                else if (state == STATE_DB_QUOTE)
                    state = STATE_NORMAL;
            } else if (command[pos] == '\'') {
                if (state == STATE_NORMAL)
                    state = STATE_QUOTE;
                else if (state == STATE_QUOTE)
                    state = STATE_NORMAL;
            }
        }
        pos ++;
    }
}

const std::string &Tokenizer::get_command() { return command; }

} // namespace tsh
