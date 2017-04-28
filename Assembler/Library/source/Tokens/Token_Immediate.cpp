#include <Tokens/Token_Immediate.hpp>

Immediate::Immediate()
        : Token()
{

}

Immediate::Immediate(std::string &immediate, std::string &immediate_uppercase, int line_number)
        : Token(immediate, immediate_uppercase, line_number)
{

}

Token::token_type Immediate::type() const
{
        return Token::token_type::IMMEDIATE;
}
