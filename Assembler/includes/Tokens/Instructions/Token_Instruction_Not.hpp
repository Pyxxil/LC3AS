#ifndef PROJECT_TOKEN_OPERATOR_NOT_HPP
#define PROJECT_TOKEN_OPERATOR_NOT_HPP

#include "../Token_Instruction.hpp"

class Not : public Instruction
{
public:
        Not(std::string &oper, int line_number = 0) : Instruction(oper, line_number)
        {}

        virtual Token::token_type type() const override
        {
                return Token::token_type::OP_NOT;
        }

        std::int32_t assemble(std::vector<Token *> &tokens, bool *orig_seen, bool *end_seen) override
        {
                if (tokens.size() != 3) {
                        return -1;
                }

                if (!*orig_seen) {
                        expected(".ORIG directive");
                        return -1;
                } else if (*end_seen) {
                        WARNING("NOT after .END directive. It will be ignored");
                        return 0;
                }

                if (tokens[1]->type() != Token::REGISTER) {

                }

                return 1;
        }
};

#endif //PROJECT_TOKEN_OPERATOR_NOT_HPP
