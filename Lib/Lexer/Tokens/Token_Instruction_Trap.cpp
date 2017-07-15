#include "Tokens/Token_Instruction_Trap.hpp"

#include <iomanip>
#include <sstream>
#include <bitset>

#include "Tokens/Token_Immediate.hpp"

Trap::Trap(std::string &instruction,
           std::string &instruction_uppercase,
           std::string &t_file,
           size_t line_number,
           size_t column)
        : Instruction(instruction, instruction_uppercase, t_file, line_number, column)
{

}

std::int32_t Trap::assemble(std::vector<std::shared_ptr<Token>> &tokens,
                            const std::map<std::string, Symbol> &symbols,
                            uint16_t program_counter)
{
        (void) symbols;
        (void) program_counter;

        if (!is_valid) {
                return -1;
        }

        assembled.emplace_back(
                static_cast<uint16_t>(0xF000 |
                        (std::static_pointer_cast<Immediate>(tokens.at(1))->value & 0xFF))
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
                tokens.at(1)->expected("valid trap vector");
                return (is_valid = false);
        }

        if (!tokens.at(1)->is_valid) {
                return (is_valid = false);
        }

        if (std::static_pointer_cast<Immediate>(tokens.at(1))->value > 0xFF) {
                tokens.at(1)->requires_too_many_bits(8, UNSIGNED, this, std::map<std::string, Symbol>());
                return (is_valid = false);
        }

        return is_valid;
}

std::int32_t Trap::guess_memory_size(std::vector<std::shared_ptr<Token>> &tokens) const
{
        (void) tokens;
        return static_cast<std::int32_t>(is_valid);
}

std::string Trap::disassemble(uint16_t &program_counter,
                              const std::string &symbol,
                              int width) const
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
                << ' ' << std::left << std::setfill(' ') << std::setw(width) << symbol
                // Instruction itself
                << " TRAP 0x" << std::right << std::hex << std::uppercase << std::setfill('0') << std::setw(2)
                << (assembled.front() & 0xFF)

#ifdef INCLUDE_ADDONS
                << '\t' << file
#endif
                << '\n';

        ++program_counter;

        return stream.str();
}

Token::token_type Trap::type() const
{
        return Token::token_type::OP_TRAP;
}
