#ifndef PROJECT_TOKEN_TRAP_PUTSP_HPP
#define PROJECT_TOKEN_TRAP_PUTSP_HPP

#include "Tokens/Token_Instruction.hpp"

class Putsp : public Instruction
{
public:
        Putsp(std::string &oper, int line_number = 0);

        virtual Token::token_type type() const override;
        virtual int32_t assemble(std::vector<std::shared_ptr<Token>> &tokens, Assembler &assembler) override;
};

#endif //PROJECT_TOKEN_TRAP_PUTSP_HPP
