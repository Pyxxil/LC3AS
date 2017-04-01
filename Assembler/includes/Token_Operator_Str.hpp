#ifndef PROJECT_TOKEN_OPERATOR_STR_HPP
#define PROJECT_TOKEN_OPERATOR_STR_HPP

#include "Token_Operator.hpp"

class Str : public Operator
{
public:
        Str(std::string &oper, int line_number = 0) : Operator(oper, line_number)
        {}

        Token::token_type type() override
        {
                return Token::token_type::OP_STR;
        }

        uint64_t requires() override
        {
                return static_cast<uint64_t>(Token::token_type::REGISTER | Token::token_type::IMMEDIATE);
        }
};


#endif //PROJECT_TOKEN_OPERATOR_STR_HPP
