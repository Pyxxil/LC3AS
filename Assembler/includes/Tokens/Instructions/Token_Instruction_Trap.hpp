#ifndef PROJECT_TOKEN_OPERATOR_TRAP_HPP
#define PROJECT_TOKEN_OPERATOR_TRAP_HPP

#include "../Token_Instruction.hpp"

class Trap : public Instruction
{
public:
        Trap(std::string &word, int line_number = 0);

        virtual Token::token_type type() const override;
};

#endif //PROJECT_TOKEN_OPERATOR_TRAP_HPP
