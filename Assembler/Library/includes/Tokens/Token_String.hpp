#ifndef PROJECT_TOKEN_STRING_HPP
#define PROJECT_TOKEN_STRING_HPP

#include "Token.hpp"

class String : public Token
{
public:
        String(std::string &string, int line_number = 0);

        virtual Token::token_type type() const override;
};

#endif //PROJECT_TOKEN_STRING_HPP
