#include "Tokens/Instructions/Token_Instruction_Sti.hpp"

#include <algorithm>
#include <iomanip>
#include <sstream>

#include "Tokens/Token_Immediate.hpp"
#include "Tokens/Token_Register.hpp"
#include "Tokens/Token_Label.hpp"
#include "Assembler.hpp"

Sti::Sti(std::string &oper, std::string &token_uppercase, int line_number)
        : Instruction(oper, token_uppercase, line_number)
{}

int32_t Sti::assemble(std::vector<std::shared_ptr<Token>> &tokens, Assembler &assembler)
{
        if (!is_valid) {
                return 0;
        }

        int offset = 0;

        if (tokens.at(2)->type() == Token::LABEL) {
                const auto &&symbol = std::find_if(
                        assembler.symbols.cbegin(),
                        assembler.symbols.cend(),
                        [&tokens](auto &&sym) -> bool
                        {
                                return sym.second->word == tokens.at(2)->word;
                        }
                );

                if (symbol == assembler.symbols.end()) {
                        std::static_pointer_cast<Label>(tokens.at(2))->not_found();
                        return -1;
                }

                offset = static_cast<int>(symbol->second->address) -
                         (static_cast<int>(assembler.internal_program_counter) + 1);
        } else {
                offset = std::static_pointer_cast<Immediate>(tokens.at(2))->immediate;
        }

        if (offset > 255 || offset < -256) {
                tokens.at(2)->expected("9 bit offset");
                return -1;
        }

        assembled.emplace_back(static_cast<std::uint16_t >(0xB000 |
                ((std::static_pointer_cast<Register>(tokens.at(1))->reg & 7) << 9) |
                (offset & 0x1FF))
        );

        return 1;
}

bool Sti::valid_arguments(std::vector<std::shared_ptr<Token>> &tokens)
{
        if (tokens.size() != 3) {
                invalid_argument_count(tokens.size(), 2);
                return (is_valid = false);
        }

        if (tokens.at(1)->type() != Token::REGISTER) {
                tokens.at(1)->expected("register");
                return (is_valid = false);
        } else if (tokens.at(2)->type() != Token::LABEL && tokens.at(2)->type() != Token::IMMEDIATE) {
                tokens.at(2)->expected("label or immediate value");
                return (is_valid = false);
        }

        if (!(tokens.at(1)->is_valid && tokens.at(2)->is_valid)) {
                return (is_valid = false);
        }

        return is_valid;
}

std::int32_t Sti::guess_memory_size(std::vector<std::shared_ptr<Token>> &tokens) const
{
        (void) tokens;
        return static_cast<std::int32_t>(is_valid);
}

std::string Sti::disassemble(std::vector<std::shared_ptr<Token>> &tokens,
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
                << " STI " << tokens.at(1)->word_as_uppercase << ' ';

        ++program_counter;

        if (tokens.at(2)->type() == Token::LABEL) {
                stream << tokens.at(2)->word << '\n';
        } else {
                const auto offset = std::static_pointer_cast<Immediate>(tokens.at(2))->immediate;
                stream << "0x" << std::hex << std::setfill('0') << std::setw(4) << (offset + program_counter) << '\n';
        }

        return stream.str();
}

Token::token_type Sti::type() const
{
        return Token::token_type::OP_STI;
}
