#include "Tokens/Instructions/Token_Instruction_And.hpp"

#include <iomanip>
#include <sstream>

#include "Tokens/Token_Immediate.hpp"
#include "Tokens/Token_Register.hpp"
#include "Assembler.hpp"

And::And()
        : Instruction()
{

}

And::And(std::string &oper, std::string &token_uppercase, int line_number)
        : Instruction(oper, token_uppercase, line_number)
{}

std::int32_t And::assemble(std::vector<std::shared_ptr<Token>> &tokens, Assembler &assembler)
{
        (void) assembler;

        if (!is_valid) {
                return 0;
        }

        assembled.emplace_back(static_cast<std::uint16_t>(0x5000 |
                ((std::static_pointer_cast<Register>(tokens[1])->reg & 7) << 9) |
                ((std::static_pointer_cast<Register>(tokens[2])->reg) & 0x7) << 6)
        );

        if (tokens[3]->type() == Token::REGISTER) {
                assembled.front() |= (std::static_pointer_cast<Register>(tokens[3])->reg & 0x7);
        } else {
                assembled.front() |= 0x20 | static_cast<std::uint16_t>(
                        std::static_pointer_cast<Immediate>(tokens[3])->immediate & 0x1F);
        }

        return 1;
}

bool And::valid_arguments(std::vector<std::shared_ptr<Token>> &tokens)
{
        if (tokens.size() != 4) {
                invalid_argument_count(tokens.size(), 3);
                return (is_valid = false);
        }

        if (tokens.at(1)->type() != Token::REGISTER) {
                tokens.at(1)->expected("register");
                return (is_valid = false);
        } else if (tokens.at(2)->type() != Token::REGISTER) {
                tokens.at(2)->expected("register");
                return (is_valid = false);
        } else if (tokens.at(3)->type() != Token::REGISTER && tokens.at(3)->type() != Token::IMMEDIATE) {
                tokens.at(3)->expected("register or immediate value");
                return (is_valid = false);
        }

        if (tokens.at(3)->type() == Token::IMMEDIATE) {
                if (std::static_pointer_cast<Immediate>(tokens.at(3))->immediate > 15 ||
                    std::static_pointer_cast<Immediate>(tokens.at(3))->immediate < -16) {
                        tokens.at(3)->expected("5 bit immediate value");
                        return (is_valid = false);
                }
        }

        if (!(tokens.at(1)->is_valid && tokens.at(2)->is_valid && tokens.at(3)->is_valid)) {
                return (is_valid = false);
        }

        return is_valid;
}

std::int32_t And::guess_memory_size(std::vector<std::shared_ptr<Token>> &tokens) const
{
        (void) tokens;
        return static_cast<std::int32_t>(is_valid);
}

std::string And::disassemble(std::vector<std::shared_ptr<Token>> &tokens,
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
                << " AND " << tokens.at(1)->word_as_uppercase << ' ' << tokens.at(2)->word_as_uppercase << " #" << std::dec
                << (static_cast<std::int16_t>(std::static_pointer_cast<Immediate>(tokens.at(3))->immediate << 11) >> 11)
                << '\n';

        ++program_counter;

        return stream.str();
}

Token::token_type And::type() const
{
        return Token::OP_AND;
}
