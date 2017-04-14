#include <Assembler.hpp>
#include "Tokens/Traps/Token_Trap_Getc.hpp"

Getc::Getc(std::string &oper, int line_number)
        : Instruction(oper, line_number)
{}

Token::token_type Getc::type() const
{
        return Token::token_type::TRAP_GETC;
}

std::int32_t Getc::assemble(std::vector<std::shared_ptr<Token>> &tokens, Assembler &assembler)
{
        if (tokens.size() != 1) {
                return -1;
        }

        if (!assembler.origin_seen) {
                expected(".ORIG directive");
                return -1;
        } else if (assembler.end_seen) {
                WARNING("GETC after .END directive. It will be ignored");
                return 0;
        }

        return 1;
}
