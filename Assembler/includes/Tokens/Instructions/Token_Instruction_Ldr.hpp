#ifndef PROJECT_TOKEN_OPERATOR_LDR_HPP
#define PROJECT_TOKEN_OPERATOR_LDR_HPP

#include "../Token_Instruction.hpp"

class Ldr : public Instruction
{
public:
        Ldr(std::string &oper, int line_number = 0) : Instruction(oper, line_number)
        {}

        virtual Token::token_type type() const override
        {
                return Token::token_type::OP_LDR;
        }

        std::int32_t assemble(std::vector<Token *> &tokens, bool *orig_seen, bool *end_seen) override
        {
                if (tokens.size() != 4) {
                        return -1;
                }

                if (!*orig_seen) {
                        expected(".ORIG directive");
                        return -1;
                } else if (*end_seen) {
                        WARNING("LDR after .END directive. It will be ignored");
                        return 0;
                }

                if (tokens[1]->type() != Token::REGISTER) {
                        tokens[1]->expected("register");
                        return -1;
                } else if (tokens[2]->type() != Token::REGISTER) {
                        tokens[2]->expected("register");
                        return -1;
                } else if (tokens[3]->type() != Token::IMMEDIATE) {
                        tokens[3]->expected("immediate value");
                        return -1;
                } else if (static_cast<Immediate *>(tokens[3])->immediate > 31 ||
                        static_cast<Immediate *>(tokens[3])->immediate < -32) {
                        tokens[3]->expected("6 bit offset");
                        return -1;
                }

                return 1;
        }
};

#endif //PROJECT_TOKEN_OPERATOR_LDR_HPP
