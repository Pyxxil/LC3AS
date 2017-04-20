#include <Assembler.hpp>
#include "Tokens/Instructions/Token_Instruction_Ret.hpp"

Ret::Ret(std::string &token, int line_number)
        : Instruction(token, line_number)
{}

std::int32_t Ret::assemble(std::vector<std::shared_ptr<Token>> &tokens, Assembler &assembler)
{
        (void) tokens;
        (void) assembler;

        assembled.emplace_back(static_cast<std::uint16_t>(0xC1C0));

        return 1;
}

bool Ret::valid_arguments(std::vector<std::shared_ptr<Token>> &tokens)
{
        if (tokens.size() != 1) {
                invalid_argument_count(tokens.size(), 0);
                return (is_valid = false);
        }

        return is_valid;
}

std::int32_t Ret::guess_memory_size(std::vector<std::shared_ptr<Token>> &tokens) const
{
        (void) tokens;
        return static_cast<int32_t>(is_valid);
}

Token::token_type Ret::type() const
{
        return Token::token_type::OP_RET;
}
