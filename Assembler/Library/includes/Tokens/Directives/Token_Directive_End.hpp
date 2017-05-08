#ifndef TOKEN_DIRECTIVE_END_HPP
#define TOKEN_DIRECTIVE_END_HPP

#include "Tokens/Token_Directive.hpp"

class End : public Directive
{
public:
        End(std::string &directive, std::string &directive_uppercase, std::string &t_file, int line_number);

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

#endif // TOKEN_DIRECTIVE_END_HPP
