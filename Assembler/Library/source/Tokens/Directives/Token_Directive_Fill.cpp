#include "Tokens/Directives/Token_Directive_Fill.hpp"

#include <sstream>
#include <iomanip>

#include "Tokens/Token_Immediate.hpp"
#include "Tokens/Token_Label.hpp"

Fill::Fill(std::string &directive, std::string &directive_uppercase, int line_number)
        : Directive(directive, directive_uppercase, line_number)
{

}

std::int32_t Fill::assemble(std::vector<std::shared_ptr<Token>> &tokens,
                            const std::map<std::string, Symbol> &symbols,
                            std::uint16_t program_counter)
{
        (void) program_counter;

        if (!is_valid) {
                return 0;
        }

        if (tokens.at(1)->type() == Token::IMMEDIATE) {
                assembled.emplace_back(static_cast<std::uint16_t>(std::static_pointer_cast<
                        Immediate>(tokens.at(1))->value));
        } else if (tokens.at(1)->type() == Token::LABEL) {
                if (!symbols.count(tokens.at(1)->token)) {
                        std::static_pointer_cast<Label>(tokens.at(1))->not_found(symbols);
                        return -1;
                }

                assembled.emplace_back(symbols.find(tokens.at(1)->token)->second.address);
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

std::string Fill::disassemble(
        std::uint16_t &program_counter,
        const std::string &symbol,
        int width) const
{
        std::stringstream stream;

        int value = static_cast<int>(assembled.front());

        stream
                // Address in memory
                << '(' << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << program_counter << ')'
                // Hexadecimal representation of instruction
                << ' ' << std::hex << std::setfill('0') << std::setw(4) << value
                // Binary representation of instruction
                << ' ' << std::bitset<16>(static_cast<unsigned long long>(value))
                // Line the instruction is on
                << " (" << std::setfill(' ') << std::right << std::dec << std::setw(4) << at_line << ')'
                // Label at the current address (if any)
                << ' ' << std::left << std::setfill(' ') << std::setw(width) << symbol
                // Instruction itself
                << " .FILL 0x" << std::right << std::hex << std::setfill('0') << std::setw(4) << value << '\n';

        ++program_counter;

        return stream.str();
}

Token::token_type Fill::type() const
{
        return Token::token_type::DIR_FILL;
}
