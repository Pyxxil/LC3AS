#include "Tokens/Instructions/Token_Instruction_Lea.hpp"
#include "Tokens/Token_Register.hpp"

Lea::Lea(std::string &oper, int line_number) : Instruction(oper, line_number)
{}

std::int32_t Lea::assemble(std::vector <std::shared_ptr<Token>> &tokens, bool *orig_seen, bool *end_seen)
{
        if (tokens.size() != 3) {
                return -1;
        }

        if (!*orig_seen) {
                expected(".ORIG directive");
                return -1;
        } else if (*end_seen) {
                WARNING("LEA after .END directive. It will be ignored");
                return 0;
        }

        if (tokens[1]->type() != Token::REGISTER) {
                tokens[1]->expected("register");
                return -1;
        } else if (tokens[2]->type() != Token::LABEL) {
                tokens[2]->expected("label");
                return -1;
        }

        assembled.push_back(static_cast<std::uint16_t >(0xE000 |
                ((std::static_pointer_cast<Register>(tokens[1])->reg & 7) << 9))
        );

        return 1;
}

Token::token_type Lea::type() const
{
        return Token::token_type::OP_LEA;
}

std::vector<std::uint16_t > Lea::as_assembled()
{
        return Token::as_assembled();
}
