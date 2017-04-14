#include "Tokens/Traps/Token_Trap_In.hpp"

In::In(std::string &oper, int line_number)
        : Instruction(oper, line_number)
{}

Token::token_type In::type() const
{
        return Token::token_type::TRAP_IN;
}
