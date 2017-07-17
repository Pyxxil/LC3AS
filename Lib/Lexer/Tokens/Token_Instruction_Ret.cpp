#include "Tokens/Token_Instruction_Ret.hpp"

#include <iomanip>
#include <sstream>
#include <bitset>

Ret::Ret(std::string &instruction,
         std::string &instruction_uppercase,
         std::string &t_file,
         size_t line_number,
         size_t column)
    : Instruction(instruction, instruction_uppercase, t_file, line_number, column)
{

}

std::int32_t Ret::assemble(std::vector<std::shared_ptr<Token>> &tokens,
                           const std::map<std::string, Symbol> &symbols,
                           uint16_t program_counter)
{
    (void) tokens;
    (void) symbols;
    (void) program_counter;

    assembled.emplace_back(static_cast<uint16_t>(0xC1C0));

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

std::string Ret::disassemble(uint16_t &program_counter,
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
        << " RET"
#ifdef INCLUDE_ADDONS
        << '\t' << file
#endif
        << '\n';

    ++program_counter;

    return stream.str();
}

Token::token_type Ret::type() const
{
    return Token::token_type::OP_RET;
}
