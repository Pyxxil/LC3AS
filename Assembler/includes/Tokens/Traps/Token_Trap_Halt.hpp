#ifndef PROJECT_TOKEN_OPERATOR_HALT_HPP
#define PROJECT_TOKEN_OPERATOR_HALT_HPP

#include "../Token_Instruction.hpp"

class Halt : public Instruction
{
public:
        Halt(std::string &oper, int line_number = 0);

        virtual Token::token_type type() const override;

        std::int32_t assemble(std::vector<std::shared_ptr<Token>> &tokens, Assembler &assembler) override;
};

#endif //PROJECT_TOKEN_OPERATOR_HALT_HPP
