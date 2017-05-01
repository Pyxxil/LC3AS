#include <Tokens/Token_Immediate.hpp>

Immediate::Immediate()
        : Token()
{

}

Immediate::Immediate(std::string &immediate, std::string &immediate_uppercase, int line_number)
        : Token(immediate, immediate_uppercase, line_number)
{

}

void Immediate::out_of_range(int bits)
{
        Token::out_of_range(bits);

        std::cerr << "Immediate value " << value << " can not be represented as a " << bits << " bit signed PC offset.\n";
}

Token::token_type Immediate::type() const
{
        return Token::token_type::IMMEDIATE;
}
