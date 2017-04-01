#ifndef PROJECT_TOKEN_OPERATOR_JSR_HPP
#define PROJECT_TOKEN_OPERATOR_JSR_HPP

#include "Token_Operator.hpp"

class Jsr : public Operator
{
public:
        Jsr(std::string &oper, int line_number = 0) : Operator(oper, line_number)
        {}

        Token::token_type type() override
        {
                return Token::token_type::OP_JSR;
        }

        uint64_t requires() override
        {
                return static_cast<uint64_t>(Token::token_type::LABEL);
        }
};

#endif //PROJECT_TOKEN_OPERATOR_JSR_HPP
