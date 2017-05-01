#include "Tokens/Directives/Token_Directive_Blkw.hpp"

#include <algorithm>
#include <sstream>
#include <iomanip>
#include <bitset>

#include "Assembler.hpp"

Blkw::Blkw(std::string &directive, std::string &directive_uppercase, int line_number)
        : Directive(directive, directive_uppercase, line_number)
{

}

std::int32_t Blkw::assemble(std::vector<std::shared_ptr<Token>> &tokens, const Assembler &assembler)
{
        if (!is_valid) {
                return 0;
        }

        std::uint16_t fill = 0;

        if (tokens.size() == 3) {
                if (tokens.at(2)->type() == Token::IMMEDIATE) {
                        fill = static_cast<std::uint16_t>(std::static_pointer_cast<Immediate>(tokens.at(2))->value);
                } else if (tokens.at(2)->type() == Token::LABEL) {
                        if (!assembler.symbols.count(tokens.at(2)->token)) {
                                const std::string possible_match = assembler.check_for_symbol_match(tokens.at(2)->token);
                                std::static_pointer_cast<Label>(tokens.at(2))->not_found(possible_match);
                                return -1;
                        }

                        fill = assembler.symbols.find(tokens.at(2)->token)->second->address;
                }
        }

        for (std::uint16_t block = 0; block < std::static_pointer_cast<Immediate>(tokens.at(1))->value; ++block) {
                assembled.emplace_back(fill);
        }

        return static_cast<std::int32_t>(assembled.size());
}

bool Blkw::valid_arguments(std::vector<std::shared_ptr<Token>> &tokens)
{
        if (tokens.size() > 3 || tokens.size() == 1) {
                invalid_argument_count(tokens.size(), 1);
                return (is_valid = false);
        }

        if (tokens.at(1)->type() != Token::IMMEDIATE) {
                tokens.at(1)->expected("value value");
                return (is_valid = false);
        } else if (!tokens.at(1)->is_valid) {
                return (is_valid = false);
        }

        if (tokens.size() == 3) {
                if (tokens.at(2)->type() != Token::IMMEDIATE && tokens.at(2)->type() != Token::LABEL) {
                        tokens.at(2)->expected("label or value value");
                        return (is_valid = false);
                }

                if (!tokens.at(2)->is_valid) {
                        return (is_valid = false);
                }
        }

        return is_valid;
}

std::int32_t Blkw::guess_memory_size(std::vector<std::shared_ptr<Token>> &tokens) const
{
        return static_cast<std::int32_t>(is_valid) * std::static_pointer_cast<Immediate>(tokens.at(1))->value;
}

void Blkw::invalid_argument_count(std::size_t provided, std::size_t expected) const
{
        (void) expected;

        provided -= 1;

        std::cerr << "ERROR: ";

        if (at_line) {
                std::cerr << "Line " << std::dec << at_line << ": ";
        }

        std::cerr << ".BLKW expects 1 or 2 arguments, but " << provided << " arguments were provided.\n";
}

std::string Blkw::disassemble(std::vector<std::shared_ptr<Token>> &tokens,
                              std::uint16_t &program_counter,
                              const std::string &symbol,
                              const Assembler &assembler) const
{
        const auto        &immediate = std::static_pointer_cast<Immediate>(tokens.at(1));
        std::stringstream stream;

        std::uint16_t fill = 0;

        if (tokens.size() > 2) {
                if (tokens.at(2)->type() == Token::IMMEDIATE) {
                        fill = static_cast<std::uint16_t>(std::static_pointer_cast<Immediate>(tokens.at(2))->value);
                } else {
                        fill = assembler.symbols.find(tokens.at(2)->token)->second->address;
                }
        }

        stream
                // Address in memory
                << '(' << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << program_counter << ')'
                // Hexadecimal representation of instruction
                << ' ' << std::hex << std::setfill('0') << std::setw(4) << fill
                // Binary representation of instruction
                << ' ' << std::bitset<16>(fill)
                // Line the instruction is on
                << " (" << std::setfill(' ') << std::right << std::dec << std::setw(4) << at_line << ')'
                // Label at the current address (if any)
                << ' ' << std::left << std::setfill(' ') << std::setw(assembler.longest_symbol_length) << symbol
                // Instruction itself
                << " .FILL 0x" << std::right << std::hex << std::setfill('0') << std::setw(4) << fill << '\n';

        ++program_counter;

        for (int block = 1; block < immediate->value; ++block, ++program_counter) {
                stream
                        // Address in memory
                        << '(' << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << program_counter
                        << ')'
                        // Hexadecimal representation of instruction
                        << ' ' << std::hex << std::setfill('0') << std::setw(4) << fill
                        // Binary representation of instruction
                        << ' ' << std::bitset<16>(fill)
                        // Line the instruction is on
                        << " (" << std::setfill(' ') << std::right << std::dec << std::setw(4) << at_line << ')'
                        // Label at the current address (if any)
                        << ' ' << std::setfill(' ') << std::setw(assembler.longest_symbol_length) << ' '
                        // Instruction itself
                        << " .FILL 0x" << std::right << std::hex << std::setfill('0') << std::setw(4) << fill << '\n';
        }

        return stream.str();
}

Token::token_type Blkw::type() const
{
        return Token::token_type::DIR_BLKW;
}
