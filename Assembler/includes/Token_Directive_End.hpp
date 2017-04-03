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

        std::uint64_t requires() override
        { return static_cast<std::uint64_t >(Token::token_type::NONE); }
};

#endif //PROJECT_TOKEN_DIRECTIVE_END_HPP
