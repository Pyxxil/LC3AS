#ifndef PROJECT_TOKEN_OPERATOR_LDI_HPP
#define PROJECT_TOKEN_OPERATOR_LDI_HPP

#include "Tokens/Token_Instruction.hpp"

class Ldi : public Instruction
{
public:
        Ldi(std::string &oper, int line_number = 0);

        virtual Token::token_type type() const override;
        virtual int32_t assemble(std::vector<std::shared_ptr<Token>> &tokens, Assembler &assembler) override;
        virtual bool valid_arguments(std::vector<std::shared_ptr<Token>> &tokens) override;
        virtual int32_t guess_memory_size(std::vector<std::shared_ptr<Token>> &tokens) const override;
};

#endif //PROJECT_TOKEN_OPERATOR_LDI_HPP
