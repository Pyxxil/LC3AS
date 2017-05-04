#ifndef TOKEN_IMMEDIATE_HPP
#define TOKEN_IMMEDIATE_HPP

#include "Token.hpp"

class Immediate : public Token
{
public:
        Immediate();
        Immediate(std::string &immediate, std::string &immediate_uppercase, int line_number = 0);

        virtual Token::token_type type() const override;
        virtual void requires_too_many_bits(int allowed_bits, bool is_signed = true) override;

        std::int16_t value = 0;
};

#endif // TOKEN_IMMEDIATE_HPP
