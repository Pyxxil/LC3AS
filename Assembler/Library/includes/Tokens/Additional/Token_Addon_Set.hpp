#ifndef PROJECT_TOKEN_ADDON_SET_HPP
#define PROJECT_TOKEN_ADDON_SET_HPP

#include "Tokens/Token_Directive.hpp"

#include "Tokens/Instructions/Token_Instruction_And.hpp"
#include "Tokens/Instructions/Token_Instruction_Add.hpp"
#include "Tokens/Instructions/Token_Instruction_Br.hpp"
#include "Tokens/Instructions/Token_Instruction_Ld.hpp"
#include "Tokens/Immediate/Token_Immediate_Decimal.hpp"
#include "Tokens/Directives/Token_Directive_Fill.hpp"

class Set : public Directive
{
public:
        Set();
        Set(std::string &word, std::string &token_uppercase, int line_number = 0);

        virtual token_type type() const override;

        virtual std::int32_t assemble(std::vector<std::shared_ptr<Token>> &tokens, Assembler &assembler) override;
        virtual std::int32_t guess_memory_size(std::vector<std::shared_ptr<Token>> &tokens) const override;

        virtual bool valid_arguments(std::vector<std::shared_ptr<Token>> &tokens) override;

        virtual std::string disassemble(std::vector<std::shared_ptr<Token>> &tokens,
                                        std::uint16_t &program_counter,
                                        const std::string &symbol,
                                        const Assembler &assembler) const override;

private:
        std::shared_ptr<And> _and;
        std::shared_ptr<Add> add;
        std::shared_ptr<Br> br;
        std::shared_ptr<Ld> ld;
        std::shared_ptr<Fill> fill;
        std::shared_ptr<Token> decimal_zero;
        std::shared_ptr<Decimal> decimal_one;
        std::shared_ptr<Decimal> decimal_negative_two;
};

#endif //PROJECT_TOKEN_ADDON_SET_HPP
