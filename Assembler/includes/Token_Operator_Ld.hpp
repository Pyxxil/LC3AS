#ifndef PROJECT_TOKEN_OPERATOR_LD_HPP
#define PROJECT_TOKEN_OPERATOR_LD_HPP

#include "Token_Operator.hpp"

class Ld : public Operator
{
public:
        Ld(std::string &oper, int line_number = 0) : Operator(oper, line_number)
        {}

        virtual Token::token_type type() const override
        {
                return Token::token_type::OP_LD;
        }
};

#endif //PROJECT_TOKEN_OPERATOR_LD_HPP
