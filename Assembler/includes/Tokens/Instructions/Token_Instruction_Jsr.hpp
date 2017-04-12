#ifndef PROJECT_TOKEN_OPERATOR_JSR_HPP
#define PROJECT_TOKEN_OPERATOR_JSR_HPP

#include "../Token_Instruction.hpp"

class Jsr : public Instruction
{
public:
        Jsr(std::string &oper, int line_number = 0) : Instruction(oper, line_number)
        {}

        virtual Token::token_type type() const override
        {
                return Token::token_type::OP_JSR;
        }

        std::int32_t assemble(std::vector<Token *> &tokens, bool *orig_seen, bool *end_seen) override
        {
                if (tokens.size() != 2) {
                        return -1;
                }

                if (!*orig_seen) {
                        expected(".ORIG directive");
                        return -1;
                } else if (*end_seen) {
                        WARNING("JSR after .END directive. It will be ignored");
                        return 0;
                }

                if (tokens[1]->type() != Token::LABEL) {
                        tokens[2]->expected("label");
                        return -1;
                }

                assembled.push_back(static_cast<std::uint16_t >(0x4800 |
                        (static_cast<Label *>(tokens[1])->address & 0x7FF)));

                return 1;
        }

        std::vector<uint16_t, std::allocator<uint16_t>> as_assembled() override
        {
                return Token::as_assembled();
        }
};

#endif //PROJECT_TOKEN_OPERATOR_JSR_HPP
