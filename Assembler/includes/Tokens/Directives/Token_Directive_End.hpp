#ifndef PROJECT_TOKEN_DIRECTIVE_END_HPP
#define PROJECT_TOKEN_DIRECTIVE_END_HPP

#include "Token_Directive.hpp"

class End : public Directive
{
public:
        End(std::string &word, int line_number = 0) : Directive(word, line_number)
        {}

        virtual Token::token_type type() const override
        { return Token::token_type::DIR_END; }

        std::int32_t assemble(std::vector<Token *> &tokens, bool *orig_seen, bool *end_seen) override
        {
                if (tokens.size() > 1) {
                        return -1;
                }

                if (!*orig_seen) {
                        expected(".ORIG directive");
                        return -1;
                } else if (*end_seen) {
                        std::cerr << "ERROR: ";
                        if (at_line) {
                                std::cerr << "Line " << at_line << ": ";
                        }
                        WARNING("Multiple .END statements");
                        is_error = true;
                }

                *end_seen = true;
                return 0;
        }
};

#endif //PROJECT_TOKEN_DIRECTIVE_END_HPP
