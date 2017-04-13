#include <Assembler.hpp>
#include "Tokens/Instructions/Token_Instruction_Br.hpp"

Br::Br(std::string &oper, int line_number, bool n, bool z, bool p)
        : Instruction(oper, line_number), N(n), Z(z), P(p)
{}

Token::token_type Br::type() const
{
        return Token::OP_BR;
}

std::int32_t Br::assemble(std::vector<std::shared_ptr<Token>> &tokens, Assembler &assembler)
{
        if (tokens.size() != 2) {
                return -1;
        }

        if (!assembler.origin_seen) {
                expected(".ORIG directive");
                return -1;
        } else if (assembler.end_seen) {
                WARNING(".END directive before .BLKW directive, .BLKW directive will be ignored.");
                return 0;
        }

        if (tokens[1]->type() != Token::LABEL) {
                tokens[1]->expected("label");
                return -1;
        }

        assembled.push_back(
                static_cast<std::uint16_t>(0x0000 | N << 10 | Z << 9 | P << 8)
        );

        return 1;
}
