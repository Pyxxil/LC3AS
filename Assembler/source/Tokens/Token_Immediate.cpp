#include <Tokens/Token_Immediate.hpp>

Immediate::Immediate() : Token()
{}

Immediate::Immediate(std::string &word, int line_number) : Token(word, line_number)
{}

Token::token_type Immediate::type() const
{
        return Token::token_type::IMMEDIATE;
}
