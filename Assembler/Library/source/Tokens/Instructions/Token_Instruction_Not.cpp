#include "Tokens/Instructions/Token_Instruction_Not.hpp"

#include <Assembler.hpp>
#include <Tokens/Token_Register.hpp>

Not::Not(std::string &oper, int line_number)
        : Instruction(oper, line_number)
{}

std::int32_t Not::assemble(std::vector<std::shared_ptr<Token>> &tokens, Assembler &assembler)
{
        if (tokens.size() != 3) {
                invalid_argument_count(tokens.size(), 2);
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
                tokens[1]->expected("register");
                return -1;
        } else if (tokens[2]->type() != Token::REGISTER) {
                tokens[2]->expected("register");
                return -1;
        }

        if (tokens[1]->is_error || tokens[2]->is_error) {
                return 0;
        }

        assembled.push_back(
                static_cast<std::uint16_t>(0x903F | ((std::static_pointer_cast<Register>(tokens[1])->reg & 0x7) << 9) |
                        ((std::static_pointer_cast<Register>(tokens[2])->reg & 0x7) << 6)
                )
        );

        return 1;
}

Token::token_type Not::type() const
{
        return Token::token_type::OP_NOT;
}
