#ifndef PROJECT_TOKEN_TRAP_OUT_HPP
#define PROJECT_TOKEN_TRAP_OUT_HPP

#include "../Token_Instruction.hpp"

class Out : public Instruction
{
public:
        Out(std::string &oper, int line_number = 0);

        virtual Token::token_type type() const override;

        std::int32_t assemble(std::vector<std::shared_ptr<Token>> &tokens, bool *orig_seen, bool *end_seen) override;
};

#endif //PROJECT_TOKEN_TRAP_OUT_HPP
