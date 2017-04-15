#include "Tokens/Traps/Token_Trap_Out.hpp"

#include "Assembler.hpp"

Out::Out(std::string &oper, int line_number)
        : Instruction(oper, line_number)
{}

std::int32_t Out::assemble(std::vector<std::shared_ptr<Token>> &tokens, Assembler &assembler)
{
        if (tokens.size() > 1) {
                return -1;
        }

        if (!assembler.origin_seen) {
                Token::expected(".ORIG statement");
                return -1;
        } else if (assembler.end_seen) {
                WARNING("OUT after .END directive. It will be ignored");
                return 0;
        }

        assembled.push_back(static_cast<std::uint16_t>(0xF021));

        return 1;
}

Token::token_type Out::type() const
{
        return Token::token_type::TRAP_OUT;
}
