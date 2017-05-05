#ifndef TOKEN_OPERATOR_HALT_HPP
#define TOKEN_OPERATOR_HALT_HPP

#include "Tokens/Token_Instruction.hpp"

class Halt : public Instruction
{
public:
        Halt(std::string &trap, std::string &trap_uppercase, int line_number = 0);

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

#endif // TOKEN_OPERATOR_HALT_HPP
