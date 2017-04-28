#include "Tokens/Directives/Token_Directive_End.hpp"

#include <iomanip>
#include <sstream>
#include <bitset>

#include "Assembler.hpp"

End::End(std::string &directive, std::string &directive_uppercase, int line_number)
        : Directive(directive, directive_uppercase, line_number)
{

}

std::int32_t End::assemble(std::vector<std::shared_ptr<Token>> &tokens, Assembler &assembler)
{
        (void) tokens;

        assembler.end_seen = true;
        return 0;
}

bool End::valid_arguments(std::vector<std::shared_ptr<Token>> &tokens)
{
        if (tokens.size() > 1) {
                invalid_argument_count(tokens.size(), 0);
                return (is_valid = false);
        }

        return is_valid;
}

std::int32_t End::guess_memory_size(std::vector<std::shared_ptr<Token>> &tokens) const
{
        (void) tokens;
        return 0;
}

std::string End::disassemble(std::vector<std::shared_ptr<Token>> &tokens,
                             std::uint16_t &program_counter,
                             const std::string &symbol,
                             const Assembler &assembler) const
{
        (void) symbol;
        (void) tokens;

        std::stringstream stream;
        stream
                // Address in memory
                << '(' << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << program_counter << ')'
                // Hexadecimal representation of instruction
                << " 0000"
                // Binary representation of instruction
                << " 0000000000000000"
                // Line the instruction is on
                << " (" << std::setfill(' ') << std::right << std::dec << std::setw(4) << at_line << ')'
                // Label at the current address (if any)
                << ' ' << std::setfill(' ') << std::setw(assembler.longest_symbol_length) << ' '
                // Instruction itself
                << " .END\n";

        return stream.str();
}

Token::token_type End::type() const
{
        return Token::token_type::DIR_END;
}
