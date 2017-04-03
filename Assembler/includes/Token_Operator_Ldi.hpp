#ifndef PROJECT_TOKEN_OPERATOR_LDI_HPP
#define PROJECT_TOKEN_OPERATOR_LDI_HPP

#include "Token_Operator.hpp"

class Ldi : public Operator
{
public:
        Ldi(std::string &oper, int line_number = 0) : Operator(oper, line_number)
        {}

        virtual Token::token_type type() const override
        {
                return Token::token_type::OP_LDI;
        }

        std::uint64_t requires() override
        {
                return static_cast<std::uint64_t>(Token::token_type::REGISTER | Token::token_type::IMMEDIATE);
        }
};

#endif //PROJECT_TOKEN_OPERATOR_LDI_HPP
