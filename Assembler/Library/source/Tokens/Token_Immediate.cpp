#include <Tokens/Token_Immediate.hpp>

Immediate::Immediate()
        : Token()
{

}

Immediate::Immediate(std::string &immediate,
                     std::string &immediate_uppercase,
                     std::string &t_file,
                     int line_number)
        : Token(immediate, immediate_uppercase, t_file, line_number)
{

}

void Immediate::requires_too_many_bits(int allowed_bits, bool is_signed)
{
        Token::requires_too_many_bits(allowed_bits, false);

        std::cerr << "Immediate value " << value << " can not be represented as a " << allowed_bits
                  << " bit " << (is_signed ? "signed" : "unsigned") << " PC offset.\n";
}

Token::token_type Immediate::type() const
{
        return Token::token_type::IMMEDIATE;
}
