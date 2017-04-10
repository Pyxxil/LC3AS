#ifndef PROJECT_TOKEN_OPERATOR_ST_HPP
#define PROJECT_TOKEN_OPERATOR_ST_HPP

#include "Token_Operator.hpp"

class St : public Operator
{
public:
        St(std::string &oper, int line_number = 0) : Operator(oper, line_number)
        {}

        virtual Token::token_type type() const override
        {
                return Token::token_type::OP_ST;
        }
};

#endif //PROJECT_TOKEN_OPERATOR_ST_HPP
