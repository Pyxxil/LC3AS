#ifndef PROJECT_TOKEN_OPERATOR_ADD_HPP
#define PROJECT_TOKEN_OPERATOR_ADD_HPP

#include "Tokens/Token_Instruction.hpp"

class Add : public Instruction
{
public:
        Add();
        Add(std::string &oper, int line_number = 0);

        virtual Token::token_type type() const override;

        std::int32_t assemble(std::vector<std::shared_ptr<Token>> &tokens, Assembler &assembler) override;
};

#endif //PROJECT_TOKEN_OPERATOR_ADD_HPP
