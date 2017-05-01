#ifndef PROJECT_TOKEN_ADDON_LSHIFT_HPP
#define PROJECT_TOKEN_ADDON_LSHIFT_HPP

#include "Tokens/Token_Directive.hpp"
#include "Tokens/Instructions/Token_Instruction_Add.hpp"

class Lshift : public Directive
{
public:
        Lshift();
        Lshift(std::string &directive, std::string &directive_uppercase, int line_number = 0);

        virtual token_type type() const override;

        virtual std::int32_t assemble(std::vector<std::shared_ptr<Token>> &tokens, const Assembler &assembler) override;
        virtual std::int32_t guess_memory_size(std::vector<std::shared_ptr<Token>> &tokens) const override;

        virtual bool valid_arguments(std::vector<std::shared_ptr<Token>> &tokens) override;

        virtual std::string disassemble(std::vector<std::shared_ptr<Token>> &tokens,
                                        std::uint16_t &program_counter,
                                        const std::string &symbol,
                                        const Assembler &assembler) const override;

private:
        std::shared_ptr<Add> add;
};

#endif //PROJECT_TOKEN_ADDON_LSHIFT_HPP
