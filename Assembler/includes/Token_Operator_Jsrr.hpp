#ifndef PROJECT_TOKEN_OPERATOR_JSRR_HPP
#define PROJECT_TOKEN_OPERATOR_JSRR_HPP

#include "Token_Operator.hpp"

class Jsrr : public Operator
{
public:
        Jsrr(std::string &oper, int line_number = 0) : Operator(oper, line_number)
        {}

        Token::token_type type() override
        {
                return Token::token_type::OP_JSRR;
        }

        uint64_t requires() override
        {
                return static_cast<uint64_t>(Token::token_type::REGISTER);
        }
};


#endif //PROJECT_TOKEN_OPERATOR_JSRR_HPP
