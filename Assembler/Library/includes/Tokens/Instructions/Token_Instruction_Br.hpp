#ifndef PROJECT_TOKEN_OPERATOR_BR_HPP
#define PROJECT_TOKEN_OPERATOR_BR_HPP

#include "Tokens/Token_Instruction.hpp"

class Br : public Instruction
{
public:
        Br(std::string &oper, int line_number = 0, bool n = false, bool z = false, bool p = false);
        std::int32_t assemble(std::vector<std::shared_ptr<Token>> &tokens, Assembler &assembler) override;
        virtual Token::token_type type() const override;

private:
        bool N;
        bool Z;
        bool P;
};

#endif //PROJECT_TOKEN_OPERATOR_BR_HPP
