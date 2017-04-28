#include "Tokens/Instructions/Token_Instruction_Jsr.hpp"

#include <algorithm>
#include <iomanip>
#include <sstream>
#include <bitset>

#include "Tokens/Token_Immediate.hpp"
#include "Tokens/Token_Label.hpp"
#include "Assembler.hpp"

Jsr::Jsr(std::string &instruction, std::string &instruction_uppercase, int line_number)
        : Instruction(instruction, instruction_uppercase, line_number)
{

}

std::int32_t Jsr::assemble(std::vector<std::shared_ptr<Token>> &tokens, Assembler &assembler)
{
        if (!is_valid) {
                return -1;
        }

        int offset = 0;

        if (tokens.at(1)->type() == Token::LABEL) {
                const auto &&symbol = std::find_if(
                        assembler.symbols.cbegin(), assembler.symbols.cend(),
                        [&tokens](const auto &sym) -> bool
                        {
                                return sym.second->token == tokens[1]->token;
                        }
                );

                if (symbol == assembler.symbols.end()) {
                        tokens.at(1)->expected("valid label");
                        return -1;
                }

                offset = static_cast<int>(symbol->second->address) -
                         (static_cast<int>(assembler.internal_program_counter) + 1);
        } else {
                offset = std::static_pointer_cast<Immediate>(tokens.at(1))->value;
        }

        if (offset > 1023 || offset < -1024) {
                // TODO: Change this to actually tell the user what's wrong (difference wise).
                tokens.at(1)->expected("11 bit offset");
                return -1;
        }

        assembled.emplace_back(0x4800 | static_cast<std::uint16_t>(offset & 0x7FF));

        return 1;
}

bool Jsr::valid_arguments(std::vector<std::shared_ptr<Token>> &tokens)
{
        if (tokens.size() != 2) {
                invalid_argument_count(tokens.size(), 1);
                return (is_valid = false);
        }

        if (tokens.at(1)->type() != Token::LABEL && tokens.at(1)->type() != Token::IMMEDIATE) {
                tokens.at(1)->expected("label or value value");
                return (is_valid = false);
        }

        return is_valid;
}

std::int32_t Jsr::guess_memory_size(std::vector<std::shared_ptr<Token>> &tokens) const
{
        (void) tokens;
        return static_cast<std::int32_t>(is_valid);
}

std::string Jsr::disassemble(std::vector<std::shared_ptr<Token>> &tokens,
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
                << " JSR ";

        ++program_counter;

        if (tokens.at(1)->type() == Token::LABEL) {
                stream << tokens.at(1)->token << '\n';
        } else {
                const auto offset = std::static_pointer_cast<Immediate>(tokens.at(1))->value;
                stream << "0x" << std::hex << std::setfill('0') << std::setw(4) << (offset + program_counter) << '\n';
        }

        return stream.str();
}

Token::token_type Jsr::type() const
{
        return Token::token_type::OP_JSR;
}
