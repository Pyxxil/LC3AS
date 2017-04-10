#ifndef PROJECT_TOKEN_OPERATOR_NOT_HPP
#define PROJECT_TOKEN_OPERATOR_NOT_HPP

#include "Token_Operator.hpp"

class Not : public Operator
{
public:
        Not(std::string &oper, int line_number = 0) : Operator(oper, line_number)
        {}

        virtual Token::token_type type() const override
        {
                return Token::token_type::OP_NOT;
        }
};

#endif //PROJECT_TOKEN_OPERATOR_NOT_HPP
