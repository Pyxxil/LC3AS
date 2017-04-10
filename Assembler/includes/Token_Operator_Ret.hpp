#ifndef PROJECT_TOKEN_OPERATOR_RET_HPP
#define PROJECT_TOKEN_OPERATOR_RET_HPP

#include "Token_Operator.hpp"

class Ret : public Operator
{
public:
        Ret(std::string &word, int line_number = 0) : Operator(word, line_number)
        {}

        virtual Token::token_type type() const override
        {
                return Token::token_type::OP_RET;
        }
};

#endif //PROJECT_TOKEN_OPERATOR_RET_HPP
