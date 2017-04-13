#include "Tokens/Instructions/Token_Instruction_Jsr.hpp"
#include "Tokens/Token_Label.hpp"

Jsr::Jsr(std::string &oper, int line_number) : Instruction(oper, line_number)
{}

std::int32_t Jsr::assemble(std::vector <std::shared_ptr<Token>> &tokens, bool *orig_seen, bool *end_seen)
{
        if (tokens.size() != 2) {
                return -1;
        }

        if (!*orig_seen) {
                expected(".ORIG directive");
                return -1;
        } else if (*end_seen) {
                WARNING("JSR after .END directive. It will be ignored");
                return 0;
        }

        if (tokens[1]->type() != Token::LABEL) {
                tokens[1]->expected("label");
                return -1;
        }

        assembled.push_back(static_cast<std::uint16_t >(0x4800 |
                (std::static_pointer_cast<Label>(tokens[1])->address & 0x7FF)));

        return 1;
}

Token::token_type Jsr::type() const
{
        return Token::token_type::OP_JSR;
}
