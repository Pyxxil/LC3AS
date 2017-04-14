#ifndef PROJECT_TOKEN_REGISTER_HPP
#define PROJECT_TOKEN_REGISTER_HPP

#include "Token.hpp"

class Register : public Token
{
public:
        Register(std::string which, int line_number = 0);

        char reg;

        virtual Token::token_type type() const override;
};

#endif //PROJECT_TOKEN_REGISTER_HPP
