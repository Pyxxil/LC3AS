#ifndef PROJECT_TOKEN_OPERATOR_JSR_HPP
#define PROJECT_TOKEN_OPERATOR_JSR_HPP

#include "Token_Operator.hpp"

class Jsr : public Operator
{
public:
        Jsr(std::string &oper, int line_number = 0) : Operator(oper, line_number)
        {}

        virtual Token::token_type type() const override
        {
                return Token::token_type::OP_JSR;
        }
};

#endif //PROJECT_TOKEN_OPERATOR_JSR_HPP
