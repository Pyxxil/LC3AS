#ifndef PROJECT_TOKEN_OPERATOR_JSRR_HPP
#define PROJECT_TOKEN_OPERATOR_JSRR_HPP

#include "../Token_Instruction.hpp"

class Jsrr : public Instruction
{
public:
        Jsrr(std::string &oper, int line_number = 0);

        virtual Token::token_type type() const override;
};

#endif //PROJECT_TOKEN_OPERATOR_JSRR_HPP
