#ifndef PROJECT_TOKEN_TRAP_OUT_HPP
#define PROJECT_TOKEN_TRAP_OUT_HPP

#include "Token_Operator.hpp"

class Out : public Operator
{
public:
        Out(std::string &oper, int line_number = 0) : Operator(oper, line_number)
        {}

        virtual Token::token_type type() const override
        {
                return Token::token_type::TRAP_OUT;
        }
};

#endif //PROJECT_TOKEN_TRAP_OUT_HPP
