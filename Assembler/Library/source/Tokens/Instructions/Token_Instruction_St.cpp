#include <Assembler.hpp>
#include "Tokens/Instructions/Token_Instruction_St.hpp"

St::St(std::string &oper, int line_number)
        : Instruction(oper, line_number)
{}

std::int32_t St::assemble(std::vector<std::shared_ptr<Token>> &tokens, Assembler &assembler)
{
        if (tokens.size() != 3) {
                return -1;
        }

        if (!assembler.origin_seen) {
                expected(".ORIG directive");
                return -1;
        } else if (assembler.end_seen) {
                WARNING("PUTS after .END directive. It will be ignored");
                return 0;
        }

        if (tokens[1]->type() != Token::REGISTER) {
                tokens[1]->expected("register");
                return -1;
        } else if (tokens[2]->type() != Token::LABEL) {
                tokens[2]->expected("label");
                return -1;
        }

        return 1;
}

Token::token_type St::type() const
{
        return Token::token_type::OP_ST;
}
