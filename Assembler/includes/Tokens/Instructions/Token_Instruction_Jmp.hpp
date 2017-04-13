#ifndef PROJECT_TOKEN_OPERATOR_JMP_HPP
#define PROJECT_TOKEN_OPERATOR_JMP_HPP

#include "../Token_Instruction.hpp"

class Jmp : public Instruction
{
public:
        Jmp(std::string &oper, int line_number = 0);

        virtual Token::token_type type() const override;
};

#endif //PROJECT_TOKEN_OPERATOR_JMP_HPP
