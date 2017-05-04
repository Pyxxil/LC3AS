#ifndef TOKEN_STRING_HPP
#define TOKEN_STRING_HPP

#include "Token.hpp"

class String : public Token
{
public:
        String(std::string &string, int line_number = 0);

        virtual Token::token_type type() const override;
};

#endif // TOKEN_STRING_HPP
