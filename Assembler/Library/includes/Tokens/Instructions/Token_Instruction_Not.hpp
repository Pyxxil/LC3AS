#ifndef TOKEN_OPERATOR_NOT_HPP
#define TOKEN_OPERATOR_NOT_HPP

#include "Tokens/Token_Instruction.hpp"

class Not : public Instruction
{
public:
        Not();
        Not(std::string &instruction, std::string &instruction_uppercase, std::string &t_file, int line_number);

        virtual Token::token_type type() const override;

        virtual std::int32_t assemble(std::vector<std::shared_ptr<Token>> &tokens,
                                      const std::map<std::string, Symbol> &symbols,
                                      std::uint16_t program_counter) override;
        virtual std::int32_t guess_memory_size(std::vector<std::shared_ptr<Token>> &tokens) const override;

        virtual bool valid_arguments(std::vector<std::shared_ptr<Token>> &tokens) override;

        virtual std::string disassemble(
                std::uint16_t &program_counter,
                const std::string &symbol,
                int width) const override;
};

#endif // TOKEN_OPERATOR_NOT_HPP
