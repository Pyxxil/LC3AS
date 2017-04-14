#include "Tokens/Instructions/Token_Instruction_Jmp.hpp"

Jmp::Jmp(std::string &oper, int line_number)
        : Instruction(oper, line_number)
{}

Token::token_type Jmp::type() const
{
        return Token::token_type::OP_JMP;
}
