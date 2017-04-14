#ifndef PROJECT_TOKEN_OPERATOR_STI_HPP
#define PROJECT_TOKEN_OPERATOR_STI_HPP

#include "Tokens/Token_Instruction.hpp"

class Sti : public Instruction
{
public:
        Sti(std::string &oper, int line_number = 0);

        virtual Token::token_type type() const override;
};

#endif //PROJECT_TOKEN_OPERATOR_STI_HPP
