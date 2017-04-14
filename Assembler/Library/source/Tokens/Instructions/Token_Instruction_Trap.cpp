#include "Tokens/Instructions/Token_Instruction_Trap.hpp"

Trap::Trap(std::string &word, int line_number)
        : Instruction(word, line_number)
{}

Token::token_type Trap::type() const
{
        return Token::token_type::OP_TRAP;
}
