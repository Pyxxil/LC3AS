#ifndef PROJECT_TOKEN_IMMEDIATE_HPP
#define PROJECT_TOKEN_IMMEDIATE_HPP

#include "Token.hpp"

class Immediate : public Token
{
public:
        Immediate() : Token()
        {}
        Immediate(std::string &word, int line_number = 0) : Token(word, line_number)
        {}

        std::int16_t immediate = 0;

        virtual Token::token_type type() const
        { return Token::token_type::IMMEDIATE; }
};

#endif //PROJECT_TOKEN_IMMEDIATE_HPP
