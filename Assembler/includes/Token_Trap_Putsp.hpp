#ifndef PROJECT_TOKEN_TRAP_PUTSP_HPP
#define PROJECT_TOKEN_TRAP_PUTSP_HPP

#include "Token_Operator.hpp"

class Putsp : public Operator
{
public:
        Putsp(std::string &oper, int line_number = 0) : Operator(oper, line_number)
        {}

        Token::token_type type() override
        {
                return Token::token_type::TRAP_PUTSP;
        }

        uint64_t requires() override
        {
                return static_cast<uint64_t>(Token::token_type::NONE);
        }
};

#endif //PROJECT_TOKEN_TRAP_PUTSP_HPP
