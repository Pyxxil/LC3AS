#include "Tokens/Traps/Token_Trap_Putsp.hpp"

#include <iomanip>
#include <sstream>

#include "Assembler.hpp"

Putsp::Putsp(std::string &oper, std::string &token_uppercase, int line_number)
        : Instruction(oper, token_uppercase, line_number)
{}

std::int32_t Putsp::assemble(std::vector<std::shared_ptr<Token>> &tokens, Assembler &assembler)
{
        (void) tokens;
        (void) assembler;

        if (!is_valid) {
                return -1;
        }

        assembled.emplace_back(static_cast<std::uint16_t>(0xF024));

        return 1;
}

bool Putsp::valid_arguments(std::vector<std::shared_ptr<Token>> &tokens)
{
        if (tokens.size() > 1) {
                invalid_argument_count(tokens.size(), 0);
                return (is_valid = false);
        }

        return is_valid;
}

std::int32_t Putsp::guess_memory_size(std::vector<std::shared_ptr<Token>> &tokens) const
{
        (void) tokens;
        return static_cast<std::int32_t>(is_valid);
}

std::string Putsp::disassemble(std::vector<std::shared_ptr<Token>> &tokens,
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
                << " F024"
                // Binary representation of instruction
                << " 1111000000100100"
                // Line the instruction is on
                << " (" << std::setfill(' ') << std::right << std::dec << std::setw(4) << at_line << ')'
                // Label at the current address (if any)
                << ' ' << std::left<< std::setfill(' ') << std::setw(assembler.longest_symbol_length) << symbol
                // Instruction itself
                << " PUTSP\n";

        ++program_counter;

        return stream.str();
}

Token::token_type Putsp::type() const
{
        return Token::token_type::TRAP_PUTSP;
}
