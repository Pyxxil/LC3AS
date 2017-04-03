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

        std::uint64_t requires() override
        {
                return static_cast<std::uint64_t >(Token::token_type::IMMEDIATE);
        }
};

#endif //PROJECT_TOKEN_DIRECTIVE_FILL_HPP
