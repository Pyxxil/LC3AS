#include "Tokens/Traps/Token_Trap_Halt.hpp"

#include "Assembler.hpp"

Halt::Halt(std::string &oper, int line_number)
        : Instruction(oper, line_number)
{}

std::int32_t Halt::assemble(std::vector<std::shared_ptr<Token>> &tokens, Assembler &assembler)
{
        (void) tokens;
        (void) assembler;

        if (!is_valid) {
                return -1;
        }

        assembled.emplace_back(static_cast<std::uint16_t>(0xF025));

        return 1;
}

bool Halt::valid_arguments(std::vector<std::shared_ptr<Token>> &tokens)
{
        if (tokens.size() > 1) {
                invalid_argument_count(tokens.size(), 0);
                return (is_valid = false);
        }

        return is_valid;
}

std::int32_t Halt::guess_memory_size(std::vector<std::shared_ptr<Token>> &tokens) const
{
        (void) tokens;
        return static_cast<std::int32_t>(is_valid);
}

Token::token_type Halt::type() const
{
        return Token::token_type::TRAP_HALT;
}
