#include <Assembler.hpp>
#include "Tokens/Instructions/Token_Instruction_Ret.hpp"

Ret::Ret(std::string &token, int line_number)
        : Instruction(token, line_number)
{}

std::int32_t Ret::assemble(std::vector<std::shared_ptr<Token>> &tokens, Assembler &assembler)
{
        if (tokens.size() != 1) {
                invalid_argument_count(tokens.size(), 0);
                return -1;
        }

        if (!assembler.origin_seen) {
                expected(".ORIG directive");
                return -1;
        } else if (assembler.end_seen) {
                std::cerr << "WARNING: ";
                if (at_line) {
                        std::cerr << "Line " << std::dec << at_line << ": ";
                }
                std::cerr << "RET after .END directive. It will be ignored.\n";
                return 0;
        }

        assembled.emplace_back(static_cast<std::uint16_t>(0xC1C0));

        return 1;
}

Token::token_type Ret::type() const
{
        return Token::token_type::OP_RET;
}
