#include "Tokens/Instructions/Token_Instruction_Ldr.hpp"

#include "Tokens/Token_Immediate.hpp"
#include "Tokens/Token_Register.hpp"
#include "Assembler.hpp"

Ldr::Ldr(std::string &oper, int line_number)
        : Instruction(oper, line_number)
{}

std::int32_t Ldr::assemble(std::vector<std::shared_ptr<Token>> &tokens, Assembler &assembler)
{
        if (tokens.size() != 4) {
                invalid_argument_count(tokens.size(), 3);
                return -1;
        }

        if (!assembler.origin_seen) {
                expected(".ORIG directive");
                return -1;
        } else if (assembler.end_seen) {
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
        }

        if (tokens[1]->is_error || tokens[2]->is_error || tokens[3]->is_error) {
                return 0;
        }

        if (std::static_pointer_cast<Immediate>(tokens[3])->immediate > 31 ||
            std::static_pointer_cast<Immediate>(tokens[3])->immediate < -32) {
                tokens[3]->expected("6 bit offset");
                return -1;
        }

        assembled.push_back(
                static_cast<std::uint16_t>(
                        0x6000 |
                        ((std::static_pointer_cast<Register>(tokens[1])->reg & 0x7) << 9) |
                        ((std::static_pointer_cast<Register>(tokens[2])->reg & 0x7) << 6) |
                        (std::static_pointer_cast<Immediate>(tokens[3])->immediate & 0x3F)
                )
        );

        return 1;
}

Token::token_type Ldr::type() const
{
        return Token::token_type::OP_LDR;
}
