#include "Tokens/Instructions/Token_Instruction_Trap.hpp"

#include <iomanip>
#include <sstream>
#include <bitset>

#include "Tokens/Token_Immediate.hpp"
#include "Assembler.hpp"

Trap::Trap(std::string &instruction, std::string &instruction_uppercase, int line_number)
        : Instruction(instruction, instruction_uppercase, line_number)
{

}

std::int32_t Trap::assemble(std::vector<std::shared_ptr<Token>> &tokens, Assembler &assembler)
{
        (void) assembler;

        if (!is_valid) {
                return -1;
        }

        assembled.emplace_back(static_cast<std::uint16_t>(0xF000 |
                (std::static_pointer_cast<Immediate>(tokens[1])->value & 0xFF))
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
                tokens.at(1)->expected("value value");
                return (is_valid = false);
        }

        if (!tokens.at(1)->is_valid) {
                return (is_valid = false);
        }

        if (std::static_pointer_cast<Immediate>(tokens.at(1))->value > 0xFF) {
                tokens.at(1)->expected("8 bit trap vector");
                return (is_valid = false);
        }

        if (std::static_pointer_cast<Immediate>(tokens.at(1))->value > 0x25 ||
            std::static_pointer_cast<Immediate>(tokens.at(1))->value < 0x20) {
                std::cerr << "WARNING: ";
                if (at_line) {
                        std::cerr << "Line " << std::dec << at_line << ": ";
                }
                std::cerr << "TRAP supplied " << std::static_pointer_cast<Immediate>(tokens[1])->value
                          << ", which is possibly an illegal trap vector.\n";
        }

        return is_valid;
}

std::int32_t Trap::guess_memory_size(std::vector<std::shared_ptr<Token>> &tokens) const
{
        (void) tokens;
        return static_cast<std::int32_t>(is_valid);
}

std::string Trap::disassemble(std::vector<std::shared_ptr<Token>> &tokens,
                              std::uint16_t &program_counter,
                              const std::string &symbol,
                              const Assembler &assembler) const
{
        std::stringstream stream;
        stream
                // Address in memory
                << '(' << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << program_counter << ')'
                // Hexadecimal representation of instruction
                << ' ' << std::hex << std::setfill('0') << std::setw(4) << assembled.front()
                // Binary representation of instruction
                << ' ' << std::bitset<16>(assembled.front())
                // Line the instruction is on
                << " (" << std::setfill(' ') << std::right << std::dec << std::setw(4) << at_line << ')'
                // Label at the current address (if any)
                << ' ' << std::left << std::setfill(' ') << std::setw(assembler.longest_symbol_length) << symbol
                // Instruction itself
                << " TRAP " << " 0x" << std::right << std::hex << std::uppercase << std::setfill('0') << std::setw(2)
                << std::static_pointer_cast<Immediate>(tokens.at(1))->value << '\n';

        ++program_counter;

        return stream.str();
}

Token::token_type Trap::type() const
{
        return Token::token_type::OP_TRAP;
}
