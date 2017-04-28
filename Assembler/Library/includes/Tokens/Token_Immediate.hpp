#ifndef PROJECT_TOKEN_IMMEDIATE_HPP
#define PROJECT_TOKEN_IMMEDIATE_HPP

#include "Token.hpp"

class Immediate : public Token
{
public:
        Immediate();
        Immediate(std::string &immediate, std::string &immediate_uppercase, int line_number = 0);

        virtual Token::token_type type() const override;

        std::int16_t value = 0;
};

#endif //PROJECT_TOKEN_IMMEDIATE_HPP
