#ifndef PROJECT_TOKEN_OPERATOR_AND_HPP
#define PROJECT_TOKEN_OPERATOR_AND_HPP

#include "Tokens/Token_Instruction.hpp"

class And : public Instruction
{
public:
        And();
        And(std::string &oper, std::string &token_uppercase, int line_number = 0);

        virtual Token::token_type type() const override;

        std::int32_t assemble(std::vector<std::shared_ptr<Token>> &tokens, Assembler &assembler) override;
        std::int32_t guess_memory_size(std::vector<std::shared_ptr<Token>> &tokens) const override;

        bool valid_arguments(std::vector<std::shared_ptr<Token>> &tokens) override;

        virtual std::string disassemble(std::vector<std::shared_ptr<Token>> &tokens,
                                        std::uint16_t &program_counter,
                                        const std::string &symbol,
                                        const Assembler &assembler) const override;
};

#endif //PROJECT_TOKEN_OPERATOR_AND_HPP
