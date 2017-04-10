#ifndef PROJECT_TOKEN_DIRECTIVE_BLKW_HPP
#define PROJECT_TOKEN_DIRECTIVE_BLKW_HPP

#include "Token_Directive.hpp"
#include "Token_Immediate.hpp"

class Blkw : public Directive
{
public:
        Blkw(std::string &word, int line_number = 0) : Directive(word, line_number)
        {}

        virtual Token::token_type type() const override
        { return Token::token_type::DIR_BLKW; }

        std::int32_t assemble(std::vector<Token *> &tokens, bool *orig_seen, bool *end_seen) override
        {
                if (tokens.size() != 2) {
                        return -1;
                }

                if (tokens[1]->type() != Token::IMMEDIATE) {
                        return -1;
                }

                if (!*orig_seen) {
                        expected(".ORIG directive");
                        return -1;
                } else if (*end_seen) {
                        WARNING(".END directive before .BLKW directive, .BLKW directive will be ignored.");
                        return 0;
                }

                return static_cast<Immediate *>(tokens[1])->immediate;
        }
};


#endif //PROJECT_TOKEN_DIRECTIVE_BLKW_HPP
