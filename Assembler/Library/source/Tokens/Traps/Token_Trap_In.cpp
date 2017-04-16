#include "Tokens/Traps/Token_Trap_In.hpp"

#include "Assembler.hpp"

In::In(std::string &oper, int line_number)
        : Instruction(oper, line_number)
{}

int32_t In::assemble(std::vector<std::shared_ptr<Token>> &tokens, Assembler &assembler)
{
        if (tokens.size() > 1) {
                invalid_argument_count(tokens.size(), 0);
                return -1;
        }

        if (!assembler.origin_seen) {
                expected(".ORIG statement");
                return -1;
        } else if (assembler.end_seen) {
                WARNING("IN after .END directive. It will be ignored");
                return 0;
        }

        assembled.push_back(static_cast<std::uint16_t>(0xF023));

        return 1;
}

Token::token_type In::type() const
{
        return Token::token_type::TRAP_IN;
}
