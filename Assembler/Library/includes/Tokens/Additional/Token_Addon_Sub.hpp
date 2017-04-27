#ifndef PROJECT_TOKEN_ADDON_SUB_HPP
#define PROJECT_TOKEN_ADDON_SUB_HPP

#include "Tokens/Token_Directive.hpp"

#include "Tokens/Instructions/Token_Instruction_And.hpp"
#include "Tokens/Instructions/Token_Instruction_Add.hpp"
#include "Tokens/Immediate/Token_Immediate_Decimal.hpp"
#include "Tokens/Additional/Token_Addon_Neg.hpp"

class Sub : public Directive
{
public:
        Sub();
        Sub(std::string &directive, std::string &token_uppercase, int line_number = 0);
        virtual int32_t assemble(std::vector<std::shared_ptr<Token>> &tokens, Assembler &assembler) override;
        virtual token_type type() const override;
        virtual bool valid_arguments(std::vector<std::shared_ptr<Token>> &tokens) override;
        virtual int32_t guess_memory_size(std::vector<std::shared_ptr<Token>> &tokens) const override;
        virtual std::string disassemble(std::vector<std::shared_ptr<Token>> &tokens,
                                        std::uint16_t &program_counter,
                                        const std::string &symbol,
                                        const Assembler &assembler) const override;

private:
        std::shared_ptr<And> set_zero;
        std::shared_ptr<Add> add;
        std::shared_ptr<Decimal> decimal_zero;
        std::shared_ptr<Neg> neg1;
        std::shared_ptr<Neg> neg2;
};

#endif //PROJECT_TOKEN_ADDON_SUB_HPP
