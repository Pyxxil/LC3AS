#include "Tokens/Directives/Token_Directive_Fill.hpp"

#include <algorithm>
#include <sstream>
#include <iomanip>
#include <bitset>

#include "Tokens/Token_Immediate.hpp"
#include "Tokens/Token_Label.hpp"
#include "Assembler.hpp"

Fill::Fill(std::string &directive, std::string &directive_uppercase, int line_number)
        : Directive(directive, directive_uppercase, line_number)
{

}

std::int32_t Fill::assemble(std::vector<std::shared_ptr<Token>> &tokens, Assembler &assembler)
{
        if (!is_valid) {
                return 0;
        }

        if (tokens.at(1)->type() == Token::IMMEDIATE) {
                assembled.emplace_back(static_cast<std::uint16_t>(std::static_pointer_cast<
                        Immediate>(tokens.at(1))->value));
        } else if (tokens.at(1)->type() == Token::LABEL) {
                const auto &&symbol = std::find_if(
                        assembler.symbols.cbegin(), assembler.symbols.cend(),
                        [&tokens](auto &&sym) -> bool
                        {
                                return sym.second->token == tokens.at(1)->token;
                        }
                );

                if (symbol == assembler.symbols.end()) {
                        const std::string possible_match = assembler.check_for_symbol_match(tokens.at(1)->token);
                        std::static_pointer_cast<Label>(tokens.at(1))->not_found(possible_match);
                        return -1;
                } else {
                        assembled.emplace_back(symbol->second->address);
                }
        }

        return 1;
}

bool Fill::valid_arguments(std::vector<std::shared_ptr<Token>> &tokens)
{
        if (tokens.size() < 2) {
                invalid_argument_count(tokens.size(), 1);
                return (is_valid = false);
        }

        if (tokens.at(1)->type() != Token::IMMEDIATE && tokens.at(1)->type() != Token::LABEL) {
                tokens.at(1)->expected("register or label");
                return (is_valid = false);
        } else if (!tokens.at(1)->is_valid) {
                return (is_valid = false);
        }

        return is_valid;
}

std::int32_t Fill::guess_memory_size(std::vector<std::shared_ptr<Token>> &tokens) const
{
        (void) tokens;
        return static_cast<std::int32_t>(is_valid);
}

std::string Fill::disassemble(std::vector<std::shared_ptr<Token>> &tokens,
                              std::uint16_t &program_counter,
                              const std::string &symbol,
                              const Assembler &assembler) const
{
        const auto        &immediate = std::static_pointer_cast<Immediate>(tokens.at(1));
        std::stringstream stream;

        std::uint16_t fill = 0;

        if (tokens.at(1)->type() == Token::IMMEDIATE) {
                fill = static_cast<std::uint16_t>(immediate->value);
        } else {
                const auto &&sym = std::find_if(
                        assembler.symbols.cbegin(), assembler.symbols.cend(),
                        [&tokens](auto &&_sym) -> bool
                        {
                                return _sym.second->token == tokens.at(1)->token;
                        }
                );

                fill = sym->first;
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

        return stream.str();
}

Token::token_type Fill::type() const
{
        return Token::token_type::DIR_FILL;
}
