#include "Tokens/Instructions/Token_Instruction_Jsrr.hpp"

Jsrr::Jsrr(std::string &oper, int line_number) : Instruction(oper, line_number)
{}

Token::token_type Jsrr::type() const
{
        return Token::token_type::OP_JSRR;
}
