#ifndef PROJECT_TOKEN_OPERATOR_STI_HPP
#define PROJECT_TOKEN_OPERATOR_STI_HPP

#include "Token_Operator.hpp"

class Sti : public Operator
{
public:
        Sti(std::string &oper, int line_number = 0) : Operator(oper, line_number)
        {}

        virtual Token::token_type type() const override
        {
                return Token::token_type::OP_STI;
        }
};

#endif //PROJECT_TOKEN_OPERATOR_STI_HPP
