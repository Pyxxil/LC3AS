#ifndef PROJECT_TOKEN_TRAP_GETC_HPP
#define PROJECT_TOKEN_TRAP_GETC_HPP

#include "Token_Operator.hpp"

class Getc : public Operator
{
public:
        Getc(std::string &oper, int line_number = 0) : Operator(oper, line_number)
        {}

        virtual Token::token_type type() const override
        {
                return Token::token_type::TRAP_GETC;
        }

        std::uint64_t requires() override
        {
                return static_cast<std::uint64_t>(Token::token_type::NONE);
        }
};

#endif //PROJECT_TOKEN_TRAP_GETC_HPP
