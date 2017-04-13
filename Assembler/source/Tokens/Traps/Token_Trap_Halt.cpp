#include <Assembler.hpp>
#include "Tokens/Traps/Token_Trap_Halt.hpp"

Halt::Halt(std::string &oper, int line_number) : Instruction(oper, line_number)
{}

Token::token_type Halt::type() const
{
        return Token::token_type::TRAP_HALT;
}

std::int32_t Halt::assemble(std::vector<std::shared_ptr<Token>> &tokens, Assembler &assembler)
{
        if (tokens.size() > 1) {
                for (auto it = tokens.begin() + 1; it != tokens.end(); it++) {
                        (*it)->expected("new line or comment");
                }

                return -1;
        } else if (!assembler.origin_seen) {
                Token::expected(".ORIG statement");
                return -1;
        } else if (assembler.end_seen) {
                WARNING(".END directive before .BLKW directive, .BLKW directive will be ignored.");
                return 0;
        } else {
                return 1;
        }
}
