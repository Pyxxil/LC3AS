#include <Assembler.hpp>
#include "Tokens/Instructions/Token_Instruction_Ret.hpp"

Ret::Ret(std::string &word, int line_number)
        : Instruction(word, line_number)
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
                WARNING("RET after .END directive. It will be ignored");
                return 0;
        }

        assembled.push_back(static_cast<std::uint16_t>(0xC1C0));

        return 1;
}

Token::token_type Ret::type() const
{
        return Token::token_type::OP_RET;
}
