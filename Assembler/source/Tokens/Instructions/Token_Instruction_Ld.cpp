#include <Assembler.hpp>
#include "Tokens/Instructions/Token_Instruction_Ld.hpp"
#include "Tokens/Token_Register.hpp"
#include "Tokens/Token_Label.hpp"

Ld::Ld(std::string &oper, int line_number) : Instruction(oper, line_number)
{}

std::int32_t Ld::assemble(std::vector<std::shared_ptr<Token>> &tokens, Assembler &assembler)
{
        if (tokens.size() != 3) {
                return -1;
        }

        if (!assembler.origin_seen) {
                expected(".ORIG directive");
                return -1;
        } else if (assembler.end_seen) {
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

        assembled.push_back(static_cast<std::uint16_t >(0x2000 |
                ((std::static_pointer_cast<Register>(tokens[1])->reg & 7) << 9) |
                (std::static_pointer_cast<Label>(tokens[2])->address & 0x1FF)));

        return 1;
}

Token::token_type Ld::type() const
{
        return Token::token_type::OP_LD;
}

std::vector<std::uint16_t> Ld::as_assembled()
{
        return Token::as_assembled();
}
