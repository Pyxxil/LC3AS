#ifndef PROJECT_TOKEN_OPERATOR_ADD_HPP
#define PROJECT_TOKEN_OPERATOR_ADD_HPP

#include "Token_Operator.hpp"

class Add : public Operator
{
public:
        Add(std::string &oper, int line_number = 0) : Operator(oper, line_number)
        {}

        virtual Token::token_type type() const override
        {
                return Token::token_type::OP_ADD;
        }

        std::uint64_t requires() override
        {
                return static_cast<std::uint64_t>(Token::token_type::REGISTER | Token::token_type::IMMEDIATE);
        }
};

#endif //PROJECT_TOKEN_OPERATOR_ADD_HPP
