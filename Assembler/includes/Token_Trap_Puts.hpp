#ifndef PROJECT_TOKEN_OPERATOR_PUTS_HPP
#define PROJECT_TOKEN_OPERATOR_PUTS_HPP

#include "Token_Operator.hpp"

class Puts : public Operator
{
public:
        Puts(std::string &oper, int line_number = 0) : Operator(oper, line_number)
        {}

        virtual Token::token_type type() const override
        {
                return Token::token_type::TRAP_PUTS;
        }
};

#endif //PROJECT_TOKEN_OPERATOR_PUTS_HPP
