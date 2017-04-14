#include "Tokens/Instructions/Token_Instruction_Ldi.hpp"

Ldi::Ldi(std::string &oper, int line_number)
        : Instruction(oper, line_number)
{}

Token::token_type Ldi::type() const
{
        return Token::token_type::OP_LDI;
}
