#include "Tokens/Instructions/Token_Instruction_Ld.hpp"

#include <algorithm>
#include <iomanip>
#include <sstream>
#include <bitset>

#include "Tokens/Token_Immediate.hpp"
#include "Tokens/Token_Register.hpp"
#include "Tokens/Token_Label.hpp"
#include "Assembler.hpp"

Ld::Ld(std::string &instruction, std::string &instruction_uppercase, int line_number)
        : Instruction(instruction, instruction_uppercase, line_number)
{

}

std::int32_t Ld::assemble(std::vector<std::shared_ptr<Token>> &tokens, const Assembler &assembler)
{
        if (!is_valid) {
                return 0;
        }

        int offset = 0;

        if (tokens.at(2)->type() == Token::LABEL) {
                if (!assembler.symbols.count(tokens.at(2)->token)) {
                        const std::string possible_match = assembler.check_for_symbol_match(tokens.at(2)->token);
                        std::static_pointer_cast<Label>(tokens.at(2))->not_found(possible_match);
                        return -1;
                }

                offset = static_cast<int>(assembler.symbols.find(tokens.at(2)->token)->second->address) -
                         (static_cast<int>(assembler.internal_program_counter) + 1);
        } else {
                offset = std::static_pointer_cast<Immediate>(tokens.at(2))->value;
        }

        if (offset > 255 || offset < -256) {
                tokens.at(2)->out_of_range(9);
                return -1;
        }

        assembled.emplace_back(static_cast<std::uint16_t >(0x2000 |
                ((std::static_pointer_cast<Register>(tokens.at(1))->reg & 7) << 9) |
                (offset & 0x1FF))
        );

        return 1;
}

bool Ld::valid_arguments(std::vector<std::shared_ptr<Token>> &tokens)
{
        if (tokens.size() != 3) {
                invalid_argument_count(tokens.size(), 2);
                return -1;
        }

        if (tokens.at(1)->type() != Token::REGISTER) {
                tokens.at(1)->expected("register");
                return (is_valid = false);
        } else if (tokens.at(2)->type() != Token::LABEL && tokens.at(2)->type() != Token::IMMEDIATE) {
                tokens.at(2)->expected("label or value value");
                return (is_valid = false);
        }

        if (!(tokens.at(1)->is_valid && tokens.at(2)->is_valid)) {
                return (is_valid = false);
        }

        return is_valid;
}

std::int32_t Ld::guess_memory_size(std::vector<std::shared_ptr<Token>> &tokens) const
{
        (void) tokens;
        return static_cast<std::int32_t>(is_valid);
}

std::string Ld::disassemble(std::vector<std::shared_ptr<Token>> &tokens,
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
                << " LD " << tokens.at(1)->token_uppercase << ' ';

        ++program_counter;

        if (tokens.at(2)->type() == Token::LABEL) {
                stream << tokens.at(2)->token << '\n';
        } else {
                const auto offset = std::static_pointer_cast<Immediate>(tokens.at(2))->value;
                stream << "0x" << std::hex << std::setfill('0') << std::setw(4) << (offset + program_counter) << '\n';
        }

        return stream.str();
}

Token::token_type Ld::type() const
{
        return Token::token_type::OP_LD;
}
