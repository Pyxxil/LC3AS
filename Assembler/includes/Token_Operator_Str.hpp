#ifndef PROJECT_TOKEN_OPERATOR_STR_HPP
#define PROJECT_TOKEN_OPERATOR_STR_HPP

#include "Token_Operator.hpp"

class Str : public Operator
{
public:
        Str(std::string &oper, int line_number = 0) : Operator(oper, line_number)
        {}

        virtual Token::token_type type() const override
        {
                return Token::token_type::OP_STR;
        }
};

#endif //PROJECT_TOKEN_OPERATOR_STR_HPP
