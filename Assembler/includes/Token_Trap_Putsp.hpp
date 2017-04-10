#ifndef PROJECT_TOKEN_TRAP_PUTSP_HPP
#define PROJECT_TOKEN_TRAP_PUTSP_HPP

#include "Token_Operator.hpp"

class Putsp : public Operator
{
public:
        Putsp(std::string &oper, int line_number = 0) : Operator(oper, line_number)
        {}

        virtual Token::token_type type() const override
        {
                return Token::token_type::TRAP_PUTSP;
        }
};

#endif //PROJECT_TOKEN_TRAP_PUTSP_HPP
