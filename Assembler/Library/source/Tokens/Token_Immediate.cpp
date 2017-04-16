#include <Tokens/Token_Immediate.hpp>

Immediate::Immediate()
        : Token()
{}

Immediate::Immediate(std::string &token, int line_number)
        : Token(token, line_number)
{}

std::int32_t Immediate::assemble(std::vector<std::shared_ptr<Token>> &tokens, Assembler &assembler)
{
        (void) tokens;
        (void) assembler;

        expected("label, instruction, or directive");
        return -1;
}

Token::token_type Immediate::type() const
{
        return Token::token_type::IMMEDIATE;
}
