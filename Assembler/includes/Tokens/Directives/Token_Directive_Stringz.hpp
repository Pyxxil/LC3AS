#ifndef PROJECT_TOKEN_DIRECTIVE_STRINGZ_HPP
#define PROJECT_TOKEN_DIRECTIVE_STRINGZ_HPP

#include "Token_Directive.hpp"

class Stringz : public Directive
{
public:
        Stringz(std::string &word, int line_number = 0) : Directive(word, line_number)
        {}

        virtual Token::token_type type() const override
        { return Token::token_type::DIR_STRINGZ; }

        std::int32_t assemble(std::vector<Token *> &tokens, bool *orig_seen, bool *end_seen) override
        {
                if (tokens.size() != 2) {
                        return -1;
                }

                if (tokens[1]->type() != Token::_STRING) {
                        tokens[1]->expected("String literal");
                        return -1;
                }

                if (!*orig_seen) {
                        expected(".ORIG directive");
                        return -1;
                } else if (*end_seen) {
                        WARNING(".END directive before .STRINGZ directive, .STRINGZ directive will be ignored.");
                        return 0;
                }

                return static_cast<std::int32_t>(static_cast<String *>(tokens[1])->word.length() + 1);
        }
};

#endif //PROJECT_TOKEN_DIRECTIVE_STRINGZ_HPP
