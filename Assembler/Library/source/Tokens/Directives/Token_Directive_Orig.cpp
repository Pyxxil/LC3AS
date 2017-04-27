#include "Tokens/Directives/Token_Directive_Orig.hpp"

#include <sstream>
#include <algorithm>
#include <iomanip>

#include "Assembler.hpp"

Orig::Orig(std::string &token, std::string &upper_case_word, int line_number)
        : Directive(token, upper_case_word, line_number)
{}

std::int32_t Orig::assemble(std::vector<std::shared_ptr<Token>> &tokens, Assembler &assembler)
{
        if (!is_valid) {
                return -1;
        }

        origin = static_cast<std::uint16_t>(std::static_pointer_cast<Immediate>(tokens[1])->immediate);
        assembled.emplace_back(origin);

        assembler.origin_seen = true;
        return origin;
}

bool Orig::valid_arguments(std::vector<std::shared_ptr<Token>> &tokens)
{
        if (tokens.size() != 2) {
                invalid_argument_count(tokens.size(), 1);
                return (is_valid = false);
        }

        if (tokens.at(1)->type() != Token::IMMEDIATE) {
                tokens.at(1)->expected("immediate value");
                return (is_valid = false);
        } else if (!tokens.at(1)->is_valid) {
                return (is_valid = false);
        }

        return is_valid;
}

std::int32_t Orig::guess_memory_size(std::vector<std::shared_ptr<Token>> &tokens) const
{
        if (is_valid) {
                return static_cast<std::int32_t>(std::static_pointer_cast<Immediate>(tokens.at(1))->immediate);
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
        program_counter = static_cast<std::uint16_t>(immediate->immediate);

        std::stringstream stream;
        stream
                // Address in memory
                << "(0000) " << std::uppercase
                // Hexadecimal representation of instruction
                << std::hex << std::setfill('0') << std::setw(4) << immediate->immediate
                // Binary representation of instruction
                << ' ' << std::bitset<16>(static_cast<unsigned long long>(immediate->immediate))
                // Line the instruction is on
                << " (" << std::setfill(' ') << std::right << std::dec << std::setw(4) << at_line << ')'
                // Label at the current address (if any)
                << ' ' << std::setfill(' ') << std::setw(assembler.longest_symbol_length) << ' '
                // Instruction itself
                << ' ' << word_as_uppercase << " 0x" << std::hex << std::setfill('0') << std::setw(4)
                << immediate->immediate << '\n';
        return stream.str();
}

Token::token_type Orig::type() const
{
        return Token::DIR_ORIG;
}
