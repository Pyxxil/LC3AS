#include "Tokens/Directives/Token_Directive_End.hpp"

#include <iomanip>
#include <sstream>

End::End(std::string &directive, std::string &directive_uppercase, std::string &t_file, int line_number)
        : Directive(directive, directive_uppercase, t_file, line_number)
{

}

std::int32_t End::assemble(std::vector<std::shared_ptr<Token>> &tokens,
                           const std::map<std::string, Symbol> &symbols,
                           std::uint16_t program_counter)
{
        (void) tokens;
        (void) symbols;
        (void) program_counter;

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

std::string End::disassemble(std::uint16_t &program_counter,
                             const std::string &symbol,
                             int width) const
{
        (void) symbol;

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
                << ' ' << std::setfill(' ') << std::setw(width) << ' '
                // Instruction itself
                << " .END\n";

        return stream.str();
}

Token::token_type End::type() const
{
        return Token::token_type::DIR_END;
}
