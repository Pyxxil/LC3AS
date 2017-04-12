#ifndef PROJECT_TOKEN_DIRECTIVE_FILL_HPP
#define PROJECT_TOKEN_DIRECTIVE_FILL_HPP

#include "Token_Directive.hpp"

class Fill : public Directive
{
public:
        // TODO: When actually implementing this, what about allowing .FILL LABEL, which
        // TODO: would fill the address with the address of LABEL?
        Fill(std::string &word, int line_number = 0) : Directive(word, line_number)
        {}

        virtual Token::token_type type() const override
        { return Token::token_type::DIR_FILL; }

        std::int32_t assemble(std::vector<Token *> &tokens, bool *orig_seen, bool *end_seen) override
        {
                if (tokens.size() != 2) {
                        return -1;
                }

                // TODO: Change this so it accepts labels as well (just fills that area of memory with the address
                // TODO: of the label.
                if (tokens[1]->type() != Token::IMMEDIATE && tokens[1]->type() != Token::LABEL) {
                        return -1;
                }

                if (!*orig_seen) {
                        expected(".ORIG directive");
                        return -1;
                } else if (*end_seen) {
                        WARNING(".END directive before .FILL directive, .FILL directive will be ignored.");
                        return 0;
                }

                return 1;
        }
};

#endif //PROJECT_TOKEN_DIRECTIVE_FILL_HPP
