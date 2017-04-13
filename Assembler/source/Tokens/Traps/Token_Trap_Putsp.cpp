#include "Tokens/Traps/Token_Trap_Putsp.hpp"

Putsp::Putsp(std::string &oper, int line_number) : Instruction(oper, line_number)
{}

Token::token_type Putsp::type() const
{
        return Token::token_type::TRAP_PUTSP;
}
