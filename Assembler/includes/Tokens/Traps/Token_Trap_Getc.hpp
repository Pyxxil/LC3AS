#ifndef PROJECT_TOKEN_TRAP_GETC_HPP
#define PROJECT_TOKEN_TRAP_GETC_HPP

#include "../Token_Instruction.hpp"

class Getc : public Instruction
{
public:
        Getc(std::string &oper, int line_number = 0);

        virtual Token::token_type type() const override;

        std::int32_t assemble(std::vector<std::shared_ptr<Token>> &tokens, bool *orig_seen, bool *end_seen) override;
};

#endif //PROJECT_TOKEN_TRAP_GETC_HPP
