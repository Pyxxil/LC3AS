#include "Tokens/Instructions/Token_Instruction_Not.hpp"

#include <Assembler.hpp>

Not::Not(std::string &oper, int line_number) : Instruction(oper, line_number)
{}

std::int32_t Not::assemble(std::vector<std::shared_ptr<Token>> &tokens, Assembler &assembler)
{
        if (tokens.size() != 3) {
                return -1;
        }

        if (!assembler.origin_seen) {
                expected(".ORIG directive");
                return -1;
        } else if (assembler.end_seen) {
                WARNING("NOT after .END directive. It will be ignored");
                return 0;
        }

        if (tokens[1]->type() != Token::REGISTER) {
                return -1;
        }

        return 1;
}

Token::token_type Not::type() const
{
        return Token::token_type::OP_NOT;
}
