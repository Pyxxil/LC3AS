#ifndef TOKEN_OPERATOR_JSRR_HPP
#define TOKEN_OPERATOR_JSRR_HPP

#include "Tokens/Token_Instruction.hpp"

class Jsrr : public Instruction
{
public:
        Jsrr(std::string &instruction,
             std::string &instruction_uppercase,
             std::string &t_file,
             size_t line_number,
             size_t column);

        Token::token_type type() const override;

        std::int32_t assemble(std::vector<std::shared_ptr<Token>> &tokens,
                                      const std::map<std::string, Symbol> &symbols,
                                      uint16_t program_counter) override;
        std::int32_t guess_memory_size(std::vector<std::shared_ptr<Token>> &tokens) const override;

        bool valid_arguments(std::vector<std::shared_ptr<Token>> &tokens) override;

        std::string disassemble(
                uint16_t &program_counter,
                const std::string &symbol,
                int width) const override;
};

#endif // TOKEN_OPERATOR_JSRR_HPP
