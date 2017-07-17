#ifndef TOKEN_OPERATOR_LEA_HPP
#define TOKEN_OPERATOR_LEA_HPP

#include "Tokens/Token_Instruction.hpp"

class Lea: public Instruction
{
public:
    Lea(std::string &instruction,
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

    std::string disassemble(uint16_t &program_counter,
                            const std::string &symbol,
                            int width) const override;

private:
    std::shared_ptr<Token> provided;
};

#endif // TOKEN_OPERATOR_LEA_HPP
