#include "Tokens/Instructions/Token_Instruction_Ldr.hpp"
#include "Tokens/Token_Immediate.hpp"

Ldr::Ldr(std::string &oper, int line_number) : Instruction(oper, line_number)
{}

std::int32_t Ldr::assemble(std::vector <std::shared_ptr<Token>> &tokens, bool *orig_seen, bool *end_seen)
{
        if (tokens.size() != 4) {
                return -1;
        }

        if (!*orig_seen) {
                expected(".ORIG directive");
                return -1;
        } else if (*end_seen) {
                WARNING("LDR after .END directive. It will be ignored");
                return 0;
        }

        if (tokens[1]->type() != Token::REGISTER) {
                tokens[1]->expected("register");
                return -1;
        } else if (tokens[2]->type() != Token::REGISTER) {
                tokens[2]->expected("register");
                return -1;
        } else if (tokens[3]->type() != Token::IMMEDIATE) {
                tokens[3]->expected("immediate value");
                return -1;
        } else if (std::static_pointer_cast<Immediate>(tokens[3])->immediate > 31 ||
                        std::static_pointer_cast<Immediate>(tokens[3])->immediate < -32) {
                tokens[3]->expected("6 bit offset");
                return -1;
        }

        return 1;
}

Token::token_type Ldr::type() const
{
        return Token::token_type::OP_LDR;
}
