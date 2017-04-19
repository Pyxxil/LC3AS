#ifndef PROJECT_TOKEN_OPERATOR_JSR_HPP
#define PROJECT_TOKEN_OPERATOR_JSR_HPP

#include "Tokens/Token_Instruction.hpp"

class Jsr : public Instruction
{
public:
        Jsr(std::string &oper, int line_number = 0);

        virtual Token::token_type type() const override;

        std::int32_t assemble(std::vector<std::shared_ptr<Token>> &tokens, Assembler &assembler) override;

        const std::vector<uint16_t, std::allocator<uint16_t>> as_assembled() const override;
};

#endif //PROJECT_TOKEN_OPERATOR_JSR_HPP
