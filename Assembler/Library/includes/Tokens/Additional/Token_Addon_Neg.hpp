#ifndef PROJECT_TOKEN_ADDON_NEG_HPP
#define PROJECT_TOKEN_ADDON_NEG_HPP

#include "Tokens/Token_Directive.hpp"
#include "Tokens/Instructions/Token_Instruction_Not.hpp"
#include "Tokens/Instructions/Token_Instruction_Add.hpp"

class Neg : public Directive
{
public:
        Neg();
        Neg(std::string &directive, std::string &token_uppercase, int line_number = 0);
        virtual int32_t assemble(std::vector<std::shared_ptr<Token>> &tokens, Assembler &assembler) override;
        virtual token_type type() const override;
        virtual bool valid_arguments(std::vector<std::shared_ptr<Token>> &tokens) override;
        virtual int32_t guess_memory_size(std::vector<std::shared_ptr<Token>> &tokens) const override;
        virtual std::string disassemble(std::vector<std::shared_ptr<Token>> &tokens,
                                        std::uint16_t &program_counter,
                                        const std::string &symbol,
                                        const Assembler &assembler) const override;

private:
        std::shared_ptr<Not> neg;
        std::shared_ptr<Add> add;
};

#endif //PROJECT_TOKEN_ADDON_NEG_HPP
