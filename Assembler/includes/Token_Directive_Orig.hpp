#ifndef PROJECT_TOKEN_DIRECTIVE_ORIG_HPP
#define PROJECT_TOKEN_DIRECTIVE_ORIG_HPP

#include "Token_Directive.hpp"

class Orig : public Directive
{
public:
        Orig(std::string &word, int line_number = 0) : Directive(word, line_number)
        {}

        virtual Token::token_type type() const override
        { return Token::token_type::DIR_ORIG; }

        std::uint64_t requires() override
        {
                return static_cast<std::uint64_t >(Token::token_type::IMMEDIATE);
        }
};

#endif //PROJECT_TOKEN_DIRECTIVE_ORIG_HPP
