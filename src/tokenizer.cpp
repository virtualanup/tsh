#include "tokenizer.h"
namespace tsh {

// Token class definitions
Token::Token(TokenType token_type, const std::string &token_text,
             size_t start_pos, size_t end_pos,
             std::shared_ptr<std::string> command_str)
    : type(token_type), text(token_text), start(start_pos), end(end_pos),
      command(command_str) {}

void Token::print() {
    // print information about the token
    std::cout << TokenNames[static_cast<int>(type)] << "\t\t" << '"' << text
              << '"' << "\t\t[" << start << " , " << end << ")"<<std::endl;
}
Token::~Token() {}

// Tokenizer class definitions
Tokenizer::Tokenizer() : command(new std::string) { reset(); }
Tokenizer::~Tokenizer() {}

void Tokenizer::add_string(const std::string &str) {
    // Add a string to the tokenizer. This will update the state of the
    // tokenizer so that if current string ends in open quote, we ask the
    // user for more input

    *command += str;

    while (inppos < command->size()) {
        if (inppos == 0 || (*command)[inppos - 1] != '\\') {
            if ((*command)[inppos] == '"') {
                if (state == STATE_NORMAL)
                    state = STATE_DB_QUOTE;
                else if (state == STATE_DB_QUOTE)
                    state = STATE_NORMAL;
            } else if ((*command)[inppos] == '\'') {
                if (state == STATE_NORMAL)
                    state = STATE_QUOTE;
                else if (state == STATE_QUOTE)
                    state = STATE_NORMAL;
            }
        }
        inppos++;
    }
}

const std::string &Tokenizer::get_command() const { return *command; }

TokenizerState Tokenizer::get_state() { return state; }

void Tokenizer::reset() {
    state = STATE_NORMAL;
    inppos = 0;
    (*command) = "";
}


std::unique_ptr<std::vector<Token>> Tokenizer::tokenize() const {
    auto tokens = std::unique_ptr<std::vector<Token>>(new std::vector<Token>);
    size_t pos = 0;
    size_t prevpos = 0;

    while (pos < command->size()) {

        // skip whitespace
        while ((*command)[pos] == '\t' || (*command)[pos] == ' ' ||
               (*command)[pos] == '\n')
            pos++;
        ;
        prevpos = pos;

        switch ((*command)[pos]) {
        case '|':
            tokens->push_back(Token(TOK_PIPE, "|", pos, pos + 1, command));
            pos++;
            break;
        case '&':
            tokens->push_back(Token(TOK_AMP, "&", pos, pos + 1, command));
            pos++;
            break;
        case ';':
            tokens->push_back(Token(TOK_SCL, ";", pos, pos + 1, command));
            pos++;
            break;
        default:
            // assume command by default
            std::string str_val;
            while (true) {
                char c = (*command)[pos];
                if (c == 0 || c == '|' || c == ' ' || c == '\t' || c == '\n' ||
                    c == '&' || c == ';') {
                    // add the current token as string
                    tokens->push_back(
                        Token(TOK_STR, str_val, prevpos, pos, command));
                    break;
                } else if (c == '\"' || c == '\'') {
                    pos += 1;
                    /*keep on iterating until end of quote is found*/
                    while (!((*command)[pos] == c &&
                             (*command)[pos - 1] != '\\')) {
                        str_val += (*command)[pos];
                        pos += 1;
                    }
                    /*skip the closing quote*/
                    pos += 1;
                } else {
                    str_val += (*command)[pos];
                    pos += 1;
                }
            }
        } // switch
    }
    tokens->push_back(Token(TOK_EOF, "", pos, pos, command));
    return tokens;
}
} // namespace tsh
