#ifndef PROJECT_TOKEN_TRAP_GETC_HPP
#define PROJECT_TOKEN_TRAP_GETC_HPP

#include "../Token_Instruction.hpp"

class Getc : public Instruction
{
public:
        Getc(std::string &oper, int line_number = 0) : Instruction(oper, line_number)
        {}

        virtual Token::token_type type() const override
        {
                return Token::token_type::TRAP_GETC;
        }

        std::int32_t assemble(std::vector<std::shared_ptr<Token>> &tokens, bool *orig_seen, bool *end_seen) override
        {
                if (tokens.size() != 1) {
                        return -1;
                }

                if (!*orig_seen) {
                        expected(".ORIG directive");
                        return -1;
                } else if (*end_seen) {
                        WARNING("GETC after .END directive. It will be ignored");
                        return 0;
                }

                return 1;
        }
};

#endif //PROJECT_TOKEN_TRAP_GETC_HPP
