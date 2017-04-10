#ifndef PROJECT_TOKEN_DIRECTIVE_ORIG_HPP
#define PROJECT_TOKEN_DIRECTIVE_ORIG_HPP

#include "Token_Directive.hpp"

class Orig : public Directive
{
public:
        Orig(std::string &word, int line_number = 0) : Directive(word, line_number)
        {}

        virtual Token::token_type type() const override
        { return Token::DIR_ORIG; }

        std::int32_t assemble(std::vector<Token *> &tokens, bool *orig_seen, bool *end_seen) override
        {
                if (tokens.size() != 2) {
                        return -1;
                }

                if (tokens[1]->type() != Token::IMMEDIATE) {
                        tokens[1]->expected("immediate value");
                }

                if (*orig_seen) {
                        std::cerr << "ERROR: ";
                        if (at_line) {
                                std::cerr << "Line " << at_line << ": ";
                        }
                        std::cerr << "Multiple .ORIG statements \n";

                        is_error = true;
                        return -1;
                } else if (*end_seen) {
                        WARNING(".ORIG directive after .END");
                }

                *orig_seen = true;
                return 0;
        }
};

#endif //PROJECT_TOKEN_DIRECTIVE_ORIG_HPP
