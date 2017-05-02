#include "Tokens/Directives/Token_Directive_Orig.hpp"

#include <algorithm>
#include <iomanip>
#include <sstream>
#include <bitset>

#include "Assembler.hpp"

Orig::Orig(std::string &directive, std::string &directive_uppercase, int line_number)
        : Directive(directive, directive_uppercase, line_number)
{

}

std::int32_t Orig::assemble(std::vector<std::shared_ptr<Token>> &tokens, const Assembler &assembler)
{
        (void) assembler;

        if (!is_valid) {
                return -1;
        }

        origin = static_cast<std::uint16_t>(std::static_pointer_cast<Immediate>(tokens.at(1))->value);
        assembled.emplace_back(origin);

        return origin;
}

bool Orig::valid_arguments(std::vector<std::shared_ptr<Token>> &tokens)
{
        if (tokens.size() != 2) {
                invalid_argument_count(tokens.size(), 1);
                return (is_valid = false);
        }

        if (tokens.at(1)->type() != Token::IMMEDIATE) {
                tokens.at(1)->expected("value value");
                return (is_valid = false);
        } else if (!tokens.at(1)->is_valid) {
                return (is_valid = false);
        }

        return is_valid;
}

std::int32_t Orig::guess_memory_size(std::vector<std::shared_ptr<Token>> &tokens) const
{
        if (is_valid) {
                return static_cast<std::int32_t>(std::static_pointer_cast<Immediate>(tokens.at(1))->value);
        } else {
                return -1;
        }
}

std::string Orig::disassemble(std::vector<std::shared_ptr<Token>> &tokens,
                              std::uint16_t &program_counter,
                              const std::string &symbol,
                              const Assembler &assembler) const
{
        // You can't have a symbol here...
        (void) symbol;

        const auto &immediate = std::static_pointer_cast<Immediate>(tokens.at(1));
        program_counter = static_cast<std::uint16_t>(immediate->value);

        std::stringstream stream;
        stream
                // Address in memory
                << "(0000) " << std::uppercase
                // Hexadecimal representation of instruction
                << std::hex << std::setfill('0') << std::setw(4) << immediate->value
                // Binary representation of instruction
                << ' ' << std::bitset<16>(static_cast<unsigned long long>(immediate->value))
                // Line the instruction is on
                << " (" << std::setfill(' ') << std::right << std::dec << std::setw(4) << at_line << ')'
                // Label at the current address (if any)
                << ' ' << std::setfill(' ') << std::setw(assembler.longest_symbol_length) << ' '
                // Instruction itself
                << ' ' << token_uppercase << " 0x" << std::hex << std::setfill('0') << std::setw(4)
                << immediate->value << '\n';
        return stream.str();
}

Token::token_type Orig::type() const
{
        return Token::DIR_ORIG;
}
