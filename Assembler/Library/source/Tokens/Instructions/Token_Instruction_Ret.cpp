#include "Tokens/Instructions/Token_Instruction_Ret.hpp"

#include <iomanip>
#include <sstream>
#include <bitset>

#include "Assembler.hpp"

Ret::Ret(std::string &instruction, std::string &instruction_uppercase, int line_number)
        : Instruction(instruction, instruction_uppercase, line_number)
{

}

std::int32_t Ret::assemble(std::vector<std::shared_ptr<Token>> &tokens, const Assembler &assembler)
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

std::string Ret::disassemble(std::vector<std::shared_ptr<Token>> &tokens,
                             std::uint16_t &program_counter,
                             const std::string &symbol,
                             const Assembler &assembler) const
{
        (void) tokens;

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
                << " RET\n";

        ++program_counter;

        return stream.str();
}

Token::token_type Ret::type() const
{
        return Token::token_type::OP_RET;
}
