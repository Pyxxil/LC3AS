#include "Tokens/Instructions/Token_Instruction_Trap.hpp"

#include "Tokens/Token_Immediate.hpp"
#include "Assembler.hpp"

Trap::Trap(std::string &token, int line_number)
        : Instruction(token, line_number)
{}

std::int32_t Trap::assemble(std::vector<std::shared_ptr<Token>> &tokens, Assembler &assembler)
{
        (void) assembler;

        if (!is_valid) {
                return -1;
        }

        assembled.emplace_back(
                static_cast<std::uint16_t>(
                        0xF000 |
                        (std::static_pointer_cast<Immediate>(tokens[1])->immediate & 0xFF)
                )
        );

        return 1;
}

bool Trap::valid_arguments(std::vector<std::shared_ptr<Token>> &tokens)
{
        if (tokens.size() != 2) {
                invalid_argument_count(tokens.size(), 1);
                return (is_valid = false);
        }

        if (tokens.at(1)->type() != Token::IMMEDIATE) {
                tokens.at(1)->expected("immediate value");
                return (is_valid = false);
        }

        if (!tokens.at(1)->is_valid) {
                return (is_valid = false);
        }

        if (std::static_pointer_cast<Immediate>(tokens.at(1))->immediate > 0xFF) {
                tokens.at(1)->expected("8 bit trap vector");
                return (is_valid = false);
        }

        if (std::static_pointer_cast<Immediate>(tokens.at(1))->immediate > 0x25 ||
            std::static_pointer_cast<Immediate>(tokens.at(1))->immediate < 0x20) {
                std::cerr << "WARNING: ";
                if (at_line) {
                        std::cerr << "Line " << std::dec << at_line << ": ";
                }
                std::cerr << "TRAP supplied " << std::static_pointer_cast<Immediate>(tokens[1])->immediate
                          << ", which is possibly an illegal trap vector.\n";
        }

        return is_valid;
}

std::int32_t Trap::guess_memory_size(std::vector<std::shared_ptr<Token>> &tokens) const
{
        (void) tokens;
        return static_cast<std::int32_t>(is_valid);
}

Token::token_type Trap::type() const
{
        return Token::token_type::OP_TRAP;
}
