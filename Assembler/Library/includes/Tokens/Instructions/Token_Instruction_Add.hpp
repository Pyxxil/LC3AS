#ifndef TOKEN_OPERATOR_ADD_HPP
#define TOKEN_OPERATOR_ADD_HPP

#include "Tokens/Token_Instruction.hpp"

class Add : public Instruction
{
public:
        Add();
        Add(std::string &instruction, std::string &instruction_uppercase, int line_number = 0);

        virtual Token::token_type type() const override;

        virtual std::int32_t assemble(std::vector<std::shared_ptr<Token>> &tokens, const Assembler &assembler) override;
        virtual std::int32_t guess_memory_size(std::vector<std::shared_ptr<Token>> &tokens) const override;

        virtual bool valid_arguments(std::vector<std::shared_ptr<Token>> &tokens) override;

        virtual std::string disassemble(std::vector<std::shared_ptr<Token>> &tokens,
                                        std::uint16_t &program_counter,
                                        const std::string &symbol,
                                        const Assembler &assembler) const override;
};

#endif // TOKEN_OPERATOR_ADD_HPP
