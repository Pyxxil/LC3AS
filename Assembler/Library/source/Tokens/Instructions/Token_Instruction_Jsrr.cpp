#include "Tokens/Instructions/Token_Instruction_Jsrr.hpp"

#include "Tokens/Token_Register.hpp"
#include "Assembler.hpp"

Jsrr::Jsrr(std::string &oper, int line_number)
        : Instruction(oper, line_number)
{}

std::int32_t Jsrr::assemble(std::vector<std::shared_ptr<Token>> &tokens, Assembler &assembler)
{
        if (tokens.size() != 2) {
                invalid_argument_count(tokens.size(), 1);
                return -1;
        }

        if (!assembler.origin_seen) {
                expected(".ORIG directive");
                return -1;
        } else if (assembler.end_seen) {
                WARNING(".END directive before .BLKW directive, .BLKW directive will be ignored.");
                return 0;
        }

        if (tokens[1]->type() != Token::REGISTER) {
                tokens[1]->expected("register");
                return -1;
        } else if (tokens[1]->is_error) {
                return 0;
        }

        assembled.push_back(static_cast<std::uint16_t>(0x4000 |
                (std::static_pointer_cast<Register>(tokens[1])->reg  << 5))
        );

        return 1;
}

Token::token_type Jsrr::type() const
{
        return Token::token_type::OP_JSRR;
}
