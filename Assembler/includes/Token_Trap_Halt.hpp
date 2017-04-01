#ifndef PROJECT_TOKEN_OPERATOR_HALT_HPP
#define PROJECT_TOKEN_OPERATOR_HALT_HPP

#include "Token_Operator.hpp"

class Halt : public Operator
{
public:
        Halt(std::string &oper, int line_number = 0) : Operator(oper, line_number)
        {}

        Token::token_type type() override
        {
                return Token::token_type::TRAP_HALT;
        }

        uint64_t requires() override
        {
                return static_cast<uint64_t>(Token::token_type::NONE);
        }
};

#endif //PROJECT_TOKEN_OPERATOR_HALT_HPP
