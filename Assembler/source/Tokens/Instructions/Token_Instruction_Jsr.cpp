#include <Assembler.hpp>
#include "Tokens/Instructions/Token_Instruction_Jsr.hpp"
#include "Tokens/Token_Label.hpp"

Jsr::Jsr(std::string &oper, int line_number) : Instruction(oper, line_number)
{}

std::int32_t Jsr::assemble(std::vector<std::shared_ptr<Token>> &tokens, Assembler &assembler)
{
        if (tokens.size() != 2) {
                return -1;
        }

        if (!assembler.origin_seen) {
                expected(".ORIG directive");
                return -1;
        } else if (assembler.end_seen) {
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

std::vector<std::uint16_t> Jsr::as_assembled()
{
        return Token::as_assembled();
}
