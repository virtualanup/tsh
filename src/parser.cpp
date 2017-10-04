#include "parser.h"

namespace tsh {

Parser::Parser() {}
Parser::~Parser() {}

std::shared_ptr<std::vector<std::shared_ptr<Job>>>
Parser::parse(const Tokenizer &tokenizer) {

    auto jobs = std::shared_ptr<std::vector<std::shared_ptr<Job>>>(
        new std::vector<std::shared_ptr<Job>>);
    // Parse the command line and keep adding the jobs
    size_t pos = 0;
    auto tokens = tokenizer.tokenize();
    while ((*tokens)[pos].type != TOK_EOF) {
        switch ((*tokens)[pos].type) {
        case TOK_SCL:
            pos++;
            continue;
        case TOK_AMP:
        case TOK_PIPE:
            // We are expecting TOK_STR.
            // Should not be here. Print error
            throw std::string("Parse error");
            break;
        default:
            // add a job
            auto curjob = std::shared_ptr<Job>(new Job);
            size_t cmd_start = (*tokens)[pos].start;

            while ((*tokens)[pos].type != TOK_SCL &&
                   (*tokens)[pos].type != TOK_EOF) {
                auto cur_command =
                    std::shared_ptr<Command>(new Command((*tokens)[pos].text));
                curjob->commands.push_back(cur_command);
                pos++;
                // get the arguments
                while ((*tokens)[pos].type == TOK_STR) {
                    cur_command->arguments.push_back((*tokens)[pos].text);
                    pos++;
                }
                // check if it is a background job
                if ((*tokens)[pos].type == TOK_AMP) {
                    curjob->is_background = true;
                    pos++;
                    break;
                }
                if ((*tokens)[pos].type == TOK_PIPE) {
                    // skip the pipe
                    pos++;
                }
            }
            // get the command text for the current job
            size_t cmd_end = (*tokens)[pos].start;
            curjob->str =
                tokenizer.get_command().substr(cmd_start, cmd_end - cmd_start);
            // add the job to the list
            jobs->push_back(curjob);
        }
    }
    return jobs;
}

} // namespace tsh
