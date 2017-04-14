#ifndef PROJECT_TOKEN_OPERATOR_LD_HPP
#define PROJECT_TOKEN_OPERATOR_LD_HPP

#include "Tokens/Token_Instruction.hpp"

class Ld : public Instruction
{
public:
        Ld(std::string &oper, int line_number = 0);

        virtual Token::token_type type() const override;

        std::int32_t assemble(std::vector<std::shared_ptr<Token>> &tokens, Assembler &assembler) override;

        std::vector<uint16_t> as_assembled() override;
};

#endif //PROJECT_TOKEN_OPERATOR_LD_HPP
