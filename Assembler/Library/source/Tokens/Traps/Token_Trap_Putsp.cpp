#include "Tokens/Traps/Token_Trap_Putsp.hpp"

#include "Assembler.hpp"

Putsp::Putsp(std::string &oper, int line_number)
        : Instruction(oper, line_number)
{}

int32_t Putsp::assemble(std::vector<std::shared_ptr<Token>> &tokens, Assembler &assembler)
{
        if (tokens.size() > 1) {
                invalid_argument_count(tokens.size(), 0);
                return -1;
        }

        if (!assembler.origin_seen) {
                expected(".ORIG statement");
                return -1;
        } else if (assembler.end_seen) {
                std::cerr << "WARNING: ";
                if (at_line) {
                        std::cerr << "Line " << std::dec << at_line << ": ";
                }
                std::cerr << "PUTSP after .END directive. It will be ignored\n";
                return 0;
        }

        assembled.emplace_back(static_cast<std::uint16_t>(0xF024));

        return 1;
}

Token::token_type Putsp::type() const
{
        return Token::token_type::TRAP_PUTSP;
}
