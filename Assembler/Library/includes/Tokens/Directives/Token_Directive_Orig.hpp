#ifndef PROJECT_TOKEN_DIRECTIVE_ORIG_HPP
#define PROJECT_TOKEN_DIRECTIVE_ORIG_HPP

#include "Tokens/Token_Directive.hpp"
#include "Tokens/Token_Immediate.hpp"

class Orig : public Directive
{
public:
        Orig(std::string &word, std::string &upper_case_word, int line_number);

        std::uint16_t origin = 0x3000;

        virtual Token::token_type type() const override;

        std::int32_t assemble(std::vector<std::shared_ptr<Token>> &tokens, Assembler &assembler) override;
        virtual bool valid_arguments(std::vector<std::shared_ptr<Token>> &tokens) override;
        virtual std::int32_t guess_memory_size(std::vector<std::shared_ptr<Token>> &tokens) const override;
        std::string disassemble(std::vector<std::shared_ptr<Token>> &tokens,
                                std::uint16_t &program_counter,
                                const std::string &symbol,
                                const Assembler &assembler) const override;
};

#endif //PROJECT_TOKEN_DIRECTIVE_ORIG_HPP
