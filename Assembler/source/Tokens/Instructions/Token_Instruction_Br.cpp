#include "Tokens/Instructions/Token_Instruction_Br.hpp"

Br::Br(std::string &oper, int line_number, bool n, bool z, bool p)
        : Instruction(oper, line_number), N(n), Z(z), P(p)
{}

Token::token_type Br::type() const
{
        return Token::OP_BR;
}

std::int32_t Br::assemble(std::vector <std::shared_ptr<Token>> &tokens, bool *orig_seen, bool *end_seen)
{
        if (tokens.size() != 2) {
                return -1;
        }

        if (tokens[1]->type() != Token::LABEL) {
                return -1;
        }

        if (!*orig_seen) {
                expected(".ORIG directive");
        } else if (*end_seen) {
                WARNING(".END directive before .BLKW directive, .BLKW directive will be ignored.");
        } else {
                return 1;
        }

        return -1;
}

std::vector<std::uint16_t > Br::as_assembled()
{
        (void) N;
        (void) Z;
        (void) P;
        return Token::as_assembled();
}
