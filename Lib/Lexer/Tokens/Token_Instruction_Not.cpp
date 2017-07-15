#include "Tokens/Token_Instruction_Not.hpp"

#include <iomanip>
#include <sstream>
#include <bitset>

#include "Tokens/Token_Register.hpp"

Not::Not()
        : Instruction()
{

}

Not::Not(std::string &instruction,
         std::string &instruction_uppercase,
         std::string &t_file,
         size_t line_number,
         size_t column)
        : Instruction(instruction, instruction_uppercase, t_file, line_number, column)
{

}

Not::Not(std::string &&instruction,
         std::string &&instruction_uppercase,
         std::string &t_file,
         size_t line_number,
         size_t column)
        : Instruction(instruction, instruction_uppercase, t_file, line_number, column)
{

}

std::int32_t Not::assemble(std::vector<std::shared_ptr<Token>> &tokens,
                           const std::map<std::string, Symbol> &symbols,
                           uint16_t program_counter)
{
        (void) symbols;
        (void) program_counter;

        if (!is_valid) {
                return -1;
        }

        assembled.emplace_back(
                static_cast<uint16_t>(0x903F |
                                      ((std::static_pointer_cast<Register>(tokens.at(1))->reg & 0x7) << 9) |
                                      ((std::static_pointer_cast<Register>(tokens.at(2))->reg & 0x7) << 6))
        );

        return 1;
}

bool Not::valid_arguments(std::vector<std::shared_ptr<Token>> &tokens)
{
        if (tokens.size() != 3) {
                invalid_argument_count(tokens.size(), 2);
                return (is_valid = false);
        }

        if (tokens.at(1)->type() != Token::REGISTER) {
                tokens.at(1)->expected("register");
                return (is_valid = false);
        } else if (tokens.at(2)->type() != Token::REGISTER) {
                tokens.at(2)->expected("register");
                return (is_valid = false);
        }

        if (!(tokens.at(1)->is_valid && tokens.at(2)->is_valid)) {
                return (is_valid = false);
        }

        return is_valid;
}

std::int32_t Not::guess_memory_size(std::vector<std::shared_ptr<Token>> &tokens) const
{
        (void) tokens;
        return static_cast<std::int32_t>(is_valid);
}

std::string Not::disassemble(uint16_t &program_counter,
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
                << " NOT R" << ((assembled.front() & 0x0E00) >> 9 & 7) << " R"
                << ((assembled.front() & 0x01C0) >> 6 & 7)
#ifdef INCLUDE_ADDONS
                << '\t' << file
#endif
                << '\n';

        ++program_counter;

        return stream.str();
}

Token::token_type Not::type() const
{
        return Token::token_type::OP_NOT;
}
