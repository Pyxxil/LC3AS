#include "Tokens/Token_Instruction_Jsrr.hpp"

#include <iomanip>
#include <sstream>
#include <bitset>

#include "Tokens/Token_Register.hpp"

Jsrr::Jsrr(std::string &instruction, std::string &instruction_uppercase, std::string &t_file, int line_number)
        : Instruction(instruction, instruction_uppercase, t_file, line_number)
{

}

std::int32_t Jsrr::assemble(std::vector<std::shared_ptr<Token>> &tokens,
                            const std::map<std::string, Symbol> &symbols,
                            std::uint16_t program_counter)
{
        (void) symbols;
        (void) program_counter;

        if (!is_valid) {
                return -1;
        }

        assembled.emplace_back(
                static_cast<std::uint16_t>(0x4000 |
                        (std::static_pointer_cast<Register>(tokens.at(1))->reg << 5))
        );

        return 1;
}

bool Jsrr::valid_arguments(std::vector<std::shared_ptr<Token>> &tokens)
{
        if (tokens.size() != 2) {
                invalid_argument_count(tokens.size(), 1);
                return (is_valid = false);
        }

        if (tokens.at(1)->type() != Token::LABEL) {
                tokens.at(1)->expected("label");
                return (is_valid = false);
        }

        return is_valid;
}

std::int32_t Jsrr::guess_memory_size(std::vector<std::shared_ptr<Token>> &tokens) const
{
        (void) tokens;
        return static_cast<std::int32_t>(is_valid);
}

std::string Jsrr::disassemble(std::uint16_t &program_counter,
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
                << " JSRR R" << ((assembled.front() & 0x01C0) >> 6 & 7)
#ifdef INCLUDE_ADDONS
                << '\t' << file
#endif
                << '\n';

        ++program_counter;

        return stream.str();
}

Token::token_type Jsrr::type() const
{
        return Token::token_type::OP_JSRR;
}
