#include "Tokens/Traps/Token_Trap_Out.hpp"

#include <iomanip>
#include <sstream>

Out::Out(std::string &trap, std::string &trap_uppercase, int line_number)
        : Instruction(trap, trap_uppercase, line_number)
{

}

std::int32_t Out::assemble(std::vector<std::shared_ptr<Token>> &tokens,
                           const std::map<std::string, Symbol> &symbols,
                           std::uint16_t program_counter)
{
        (void) tokens;
        (void) symbols;
        (void) program_counter;

        if (!is_valid) {
                return -1;
        }

        assembled.emplace_back(static_cast<std::uint16_t>(0xF021));

        return 1;
}

bool Out::valid_arguments(std::vector<std::shared_ptr<Token>> &tokens)
{
        if (tokens.size() > 1) {
                invalid_argument_count(tokens.size(), 0);
                return (is_valid = false);
        }

        return is_valid;
}

std::int32_t Out::guess_memory_size(std::vector<std::shared_ptr<Token>> &tokens) const
{
        (void) tokens;
        return static_cast<std::int32_t>(is_valid);
}

std::string Out::disassemble(std::uint16_t &program_counter,
                             const std::string &symbol,
                             int width) const
{
        std::stringstream stream;
        stream
                // Address in memory
                << '(' << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << program_counter << ')'
                // Hexadecimal representation of instruction
                << " F021"
                // Binary representation of instruction
                << " 1111000000100001"
                // Line the instruction is on
                << " (" << std::setfill(' ') << std::right << std::dec << std::setw(4) << at_line << ')'
                // Label at the current address (if any)
                << ' ' << std::left << std::setfill(' ') << std::setw(width) << symbol
                // Instruction itself
                << " OUT\n";

        ++program_counter;

        return stream.str();
}

Token::token_type Out::type() const
{
        return Token::token_type::TRAP_OUT;
}
