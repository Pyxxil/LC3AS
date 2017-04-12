#ifndef PROJECT_TOKEN_OPERATOR_HALT_HPP
#define PROJECT_TOKEN_OPERATOR_HALT_HPP

#include "../Token_Instruction.hpp"

class Halt : public Instruction
{
public:
        Halt(std::string &oper, int line_number = 0) : Instruction(oper, line_number)
        {}

        virtual Token::token_type type() const override
        {
                return Token::token_type::TRAP_HALT;
        }

        std::int32_t assemble(std::vector<Token *> &tokens, bool *orig_seen, bool *end_seen) override
        {
                if (tokens.size() > 1) {
                        for (auto it = tokens.begin() + 1; it != tokens.end(); it++) {
                                (*it)->expected("new line or comment");
                        }

                        return -1;
                } else if (!*orig_seen) {
                        Token::expected(".ORIG statement");
                        return -1;
                } else if (*end_seen) {
                        WARNING(".END directive before .BLKW directive, .BLKW directive will be ignored.");
                        return 0;
                } else {
                        return 1;
                }
        }
};

#endif //PROJECT_TOKEN_OPERATOR_HALT_HPP
