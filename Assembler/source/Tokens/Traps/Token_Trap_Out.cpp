#include "Tokens/Traps/Token_Trap_Out.hpp"

Out::Out(std::string &oper, int line_number) : Instruction(oper, line_number)
{}

Token::token_type Out::type() const
{
        return Token::token_type::TRAP_OUT;
}

std::int32_t Out::assemble(std::vector <std::shared_ptr<Token>> &tokens, bool *orig_seen, bool *end_seen)
{
        if (tokens.size() != 1) {
                return -1;
        }

        if (!*orig_seen) {
                expected(".ORIG directive");
                return -1;
        } else if (*end_seen) {
                WARNING("OUT after .END directive. It will be ignored");
                return 0;
        }

        return 1;
}
