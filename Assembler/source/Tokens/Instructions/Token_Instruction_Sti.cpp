#include "Tokens/Instructions/Token_Instruction_Sti.hpp"

Sti::Sti(std::string &oper, int line_number) : Instruction(oper, line_number)
{}

Token::token_type Sti::type() const
{
        return Token::token_type::OP_STI;
}
