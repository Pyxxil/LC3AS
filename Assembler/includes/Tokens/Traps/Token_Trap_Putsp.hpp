#ifndef PROJECT_TOKEN_TRAP_PUTSP_HPP
#define PROJECT_TOKEN_TRAP_PUTSP_HPP

#include "../Token_Instruction.hpp"

class Putsp : public Instruction
{
public:
        Putsp(std::string &oper, int line_number = 0);

        virtual Token::token_type type() const override;
};

#endif //PROJECT_TOKEN_TRAP_PUTSP_HPP
