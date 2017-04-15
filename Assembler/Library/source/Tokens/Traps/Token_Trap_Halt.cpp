#include "Tokens/Traps/Token_Trap_Halt.hpp"

#include "Assembler.hpp"

Halt::Halt(std::string &oper, int line_number)
        : Instruction(oper, line_number)
{}

Token::token_type Halt::type() const
{
        return Token::token_type::TRAP_HALT;
}

std::int32_t Halt::assemble(std::vector<std::shared_ptr<Token>> &tokens, Assembler &assembler)
{
        if (tokens.size() > 1) {
                return -1;
        }

        if (!assembler.origin_seen) {
                Token::expected(".ORIG statement");
                return -1;
        } else if (assembler.end_seen) {
                WARNING("HALT after .END directive. It will be ignored");
                return 0;
        }

        assembled.push_back(static_cast<std::uint16_t>(0xF025));

        return 1;
}
