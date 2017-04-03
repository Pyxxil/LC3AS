#ifndef PROJECT_TOKEN_OPERATOR_JMP_HPP
#define PROJECT_TOKEN_OPERATOR_JMP_HPP

#include "Token_Operator.hpp"

class Jmp : public Operator
{
public:
        Jmp(std::string &oper, int line_number = 0) : Operator(oper, line_number)
        {}

        virtual Token::token_type type() const override
        {
                return Token::token_type::OP_JMP;
        }

        std::uint64_t requires() override
        {
                return static_cast<std::uint64_t>(Token::token_type::REGISTER);
        }
};

#endif //PROJECT_TOKEN_OPERATOR_JMP_HPP
