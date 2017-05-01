#include "Tokens/Instructions/Token_Instruction_Br.hpp"

#include <algorithm>
#include <iomanip>
#include <sstream>
#include <bitset>

#include "Tokens/Token_Immediate.hpp"
#include "Assembler.hpp"

Br::Br(std::string &instruction, std::string &instruction_uppercase, int line_number, bool n, bool z, bool p)
        : Instruction(instruction, instruction_uppercase, line_number), N(n), Z(z), P(p)
{

}

Br::Br(bool n, bool z, bool p)
        : Instruction(), N(n), Z(z), P(p)
{

}

std::int32_t Br::assemble(std::vector<std::shared_ptr<Token>> &tokens, const Assembler &assembler)
{
        if (!is_valid) {
                return 0;
        }

        int offset = 0;

        if (tokens.at(1)->type() == Token::LABEL) {
                if (!assembler.symbols.count(tokens.at(1)->token)) {
                        const std::string possible_match = assembler.check_for_symbol_match(tokens.at(1)->token);
                        std::static_pointer_cast<Label>(tokens.at(1))->not_found(possible_match);
                        return -1;
                }

                offset = static_cast<int>(assembler.symbols.find(tokens.at(1)->token)->second->address) -
                         (static_cast<int>(assembler.internal_program_counter) + 1);
        } else {
                offset = std::static_pointer_cast<Immediate>(tokens.at(1))->value;
        }

        if (offset > 255 || offset < -256) {
                tokens.at(1)->out_of_range(9);
                return -1;
        }

        assembled.emplace_back(static_cast<std::uint16_t>(0x0000 | N << 11 | Z << 10 | P << 9 | (offset & 0x1FF)));

        return 1;
}

bool Br::valid_arguments(std::vector<std::shared_ptr<Token>> &tokens)
{

        if (tokens.size() != 2) {
                invalid_argument_count(tokens.size(), 1);
                return (is_valid = false);
        }

        if (tokens.at(1)->type() != Token::LABEL && tokens.at(1)->type() != Token::IMMEDIATE) {
                tokens.at(1)->expected("label or value value");
                return (is_valid = false);
        } else if (!tokens.at(1)->is_valid) {
                return (is_valid = false);
        }

        return is_valid;
}

std::int32_t Br::guess_memory_size(std::vector<std::shared_ptr<Token>> &tokens) const
{
        (void) tokens;
        return static_cast<int32_t>(is_valid);
}

std::string Br::disassemble(std::vector<std::shared_ptr<Token>> &tokens,
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
                << " BR" << (N ? "n" : " ") << (Z ? "z" : " ") << (P ? "p" : " ") << ' ';

        ++program_counter;

        if (tokens.at(1)->type() == Token::LABEL) {
                stream << tokens.at(1)->token << '\n';
        } else {
                const auto offset = std::static_pointer_cast<Immediate>(tokens.at(1))->value;
                stream << "0x" << std::hex << std::setfill('0') << std::setw(4) << (offset + program_counter) << '\n';
        }

        return stream.str();
}

Token::token_type Br::type() const
{
        return Token::OP_BR;
}
