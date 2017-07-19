#ifndef TOKEN_DIRECTIVE_ORIG_HPP
#define TOKEN_DIRECTIVE_ORIG_HPP

#include "Tokens/Token_Directive.hpp"
#include "Tokens/Token_Immediate.hpp"

class Orig: public Directive
{
public:
    Orig(std::string &directive,
         std::string &directive_uppercase,
         std::string &t_file,
         size_t line_number,
         size_t column);

    Token::token_type type() const override;

    int32_t assemble(std::vector<std::shared_ptr<Token>> &tokens,
                          const std::map<std::string, Symbol> &symbols,
                          uint16_t program_counter) override;
    uint16_t guess_memory_size(std::vector<std::shared_ptr<Token>> &tokens) const override;

    bool valid_arguments(std::vector<std::shared_ptr<Token>> &tokens) override;

    std::string disassemble(
        uint16_t &program_counter,
        const std::string &symbol,
        int width) const override;

private:
    uint16_t origin = 0x3000;
};

#endif // TOKEN_DIRECTIVE_ORIG_HPP
