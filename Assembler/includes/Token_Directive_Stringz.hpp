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

        std::uint64_t requires() override
        { return static_cast<std::uint64_t >(Token::token_type::_STRING); }
};

#endif //PROJECT_TOKEN_DIRECTIVE_STRINGZ_HPP
