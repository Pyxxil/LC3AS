#ifndef PROJECT_TOKEN_OPERATOR_PUTS_HPP
#define PROJECT_TOKEN_OPERATOR_PUTS_HPP

#include "../Token_Instruction.hpp"

class Puts : public Instruction
{
public:
        Puts(std::string &oper, int line_number = 0) : Instruction(oper, line_number)
        {}

        virtual Token::token_type type() const override
        {
                return Token::token_type::TRAP_PUTS;
        }

        std::int32_t assemble(std::vector<Token *> &tokens, bool *orig_seen, bool *end_seen) override
        {
                if (tokens.size() != 1) {
                        return -1;
                }

                if (!*orig_seen) {
                        expected(".ORIG directive");
                        return -1;
                } else if (*end_seen) {
                        WARNING("PUTS after .END directive. It will be ignored");
                        return 0;
                }

                return 1;
        }
};

#endif //PROJECT_TOKEN_OPERATOR_PUTS_HPP
