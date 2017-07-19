#include "Tokens/Token_Instruction_Jmp.hpp"

#include <bitset>
#include <iomanip>
#include <sstream>

#include "Tokens/Token_Register.hpp"

Jmp::Jmp(std::string &instruction, std::string &instruction_uppercase,
         std::string &t_file, size_t line_number, size_t column)
    : Instruction(instruction, instruction_uppercase, t_file, line_number,
                  column)
{
}

int32_t Jmp::assemble(std::vector<std::shared_ptr<Token>> &tokens,
                      const std::map<std::string, Symbol> &symbols,
                      uint16_t program_counter)
{
    (void)symbols;
    (void)program_counter;

    if (!is_valid) {
        return -1;
    }

    assembled.emplace_back(static_cast<uint16_t>(
        0xC000 | (std::static_pointer_cast<Register>(tokens.at(1))->reg << 6)));

    return 1;
}

bool Jmp::valid_arguments(std::vector<std::shared_ptr<Token>> &tokens)
{
    if (tokens.size() != 2) {
        invalid_argument_count(tokens.size(), 1,
                               tokens.back()->column
                                   + tokens.back()->token.length());
        return (is_valid = false);
    }

    if (tokens.at(1)->type() != Token::REGISTER) {
        tokens.at(1)->expected("register");
        return (is_valid = false);
    }

    if (!tokens.at(1)->is_valid) {
        return (is_valid = false);
    }

    return is_valid;
}

uint16_t
Jmp::guess_memory_size(std::vector<std::shared_ptr<Token>> &tokens) const
{
    (void)tokens;
    return static_cast<uint16_t>(is_valid);
}

std::string Jmp::disassemble(uint16_t &program_counter,
                             const std::string &symbol, int width) const
{
    std::stringstream stream;
    stream
        // Address in memory
        << '(' << std::hex << std::uppercase << std::setfill('0')
        << std::setw(4) << program_counter
        << ')'
        // Hexadecimal representation of instruction
        << ' ' << std::hex << std::setfill('0') << std::setw(4)
        << assembled.front()
        // Binary representation of instruction
        << ' '
        << std::bitset<16>(assembled.front())
        // Line the instruction is on
        << " (" << std::setfill(' ') << std::right << std::dec << std::setw(4)
        << line
        << ')'
        // Label at the current address (if any)
        << ' ' << std::left << std::setfill(' ') << std::setw(width)
        << symbol
        // Instruction itself
        << " JMP R" << ((assembled.front() & 0x01C0) >> 6 & 7)
#ifdef INCLUDE_ADDONS
        << '\t' << file
#endif
        << '\n';

    ++program_counter;

    return stream.str();
}

Token::token_type Jmp::type() const { return Token::token_type::OP_JMP; }
