#ifndef TOKEN_REGISTER_HPP
#define TOKEN_REGISTER_HPP

#include "Token.hpp"

class Register : public Token
{
public:
        Register(std::string &which,
                 std::string &which_uppercase,
                 std::string &t_file,
                 int line_number);

        virtual Token::token_type type() const override;

        char reg;
};

#endif // TOKEN_REGISTER_HPP
