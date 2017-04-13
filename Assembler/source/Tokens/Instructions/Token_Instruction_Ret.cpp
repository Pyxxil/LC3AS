#include "Tokens/Instructions/Token_Instruction_Ret.hpp"

Ret::Ret(std::string &word, int line_number) : Instruction(word, line_number)
{}

std::int32_t Ret::assemble(std::vector<std::shared_ptr<Token>> &tokens, bool *orig_seen, bool *end_seen)
{
        if (tokens.size() != 1) {
                return -1;
        }

        if (!*orig_seen) {
                expected(".ORIG directive");
                return -1;
        } else if (*end_seen) {
                WARNING("RET after .END directive. It will be ignored");
                return 0;
        }

        return 1;
}

Token::token_type Ret::type() const
{
        return Token::token_type::OP_RET;
}
