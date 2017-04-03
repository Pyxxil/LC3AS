#ifndef PROJECT_TOKEN_OPERATOR_TRAP_HPP
#define PROJECT_TOKEN_OPERATOR_TRAP_HPP

#include "Token_Operator.hpp"

class Trap : public Operator
{
public:
        Trap(std::string &word, int line_number = 0) : Operator(word, line_number)
        {}

        virtual Token::token_type type() const override
        {
                return Token::token_type::OP_TRAP;
        }

        std::uint64_t requires() override
        {
                return static_cast<std::uint64_t >(Token::token_type::IMMEDIATE);
        }
};

#endif //PROJECT_TOKEN_OPERATOR_TRAP_HPP
