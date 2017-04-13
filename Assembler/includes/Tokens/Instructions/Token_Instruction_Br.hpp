#ifndef PROJECT_TOKEN_OPERATOR_BR_HPP
#define PROJECT_TOKEN_OPERATOR_BR_HPP

#include "../Token_Instruction.hpp"

class Br : public Instruction
{
public:
        Br(std::string &oper, int line_number = 0, bool n = false, bool z = false, bool p = false)
                : Instruction(oper, line_number), N(n), Z(z), P(p)
        {}

        virtual Token::token_type type() const override
        {
                return Token::OP_BR;
        }

        std::int32_t assemble(std::vector<std::shared_ptr<Token>> &tokens, bool *orig_seen, bool *end_seen) override
        {
                if (tokens.size() != 2) {
                        return -1;
                }

                if (tokens[1]->type() != Token::LABEL) {
                        return -1;
                }

                if (!*orig_seen) {
                        expected(".ORIG directive");
                } else if (*end_seen) {
                        WARNING(".END directive before .BLKW directive, .BLKW directive will be ignored.");
                } else {
                        return 1;
                }

                return -1;
        }

        std::vector<uint16_t, std::allocator<uint16_t>> as_assembled() override
        {
                (void) N;
                (void) Z;
                (void) P;
                return Token::as_assembled();
        }

private:
        bool N;
        bool Z;
        bool P;
};

#endif //PROJECT_TOKEN_OPERATOR_BR_HPP
