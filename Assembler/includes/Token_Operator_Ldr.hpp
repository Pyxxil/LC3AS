#ifndef PROJECT_TOKEN_OPERATOR_LDR_HPP
#define PROJECT_TOKEN_OPERATOR_LDR_HPP

#include "Token_Operator.hpp"

class Ldr : public Operator
{
public:
        Ldr(std::string &oper, int line_number = 0) : Operator(oper, line_number)
        {}

        virtual Token::token_type type() const override
        {
                return Token::token_type::OP_LDR;
        }
};

#endif //PROJECT_TOKEN_OPERATOR_LDR_HPP
