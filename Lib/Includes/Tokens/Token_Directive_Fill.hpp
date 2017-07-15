#ifndef TOKEN_DIRECTIVE_FILL_HPP
#define TOKEN_DIRECTIVE_FILL_HPP

#include "Tokens/Token_Directive.hpp"

class Fill : public Directive
{
public:
        Fill()
                : Directive()
        { }
        Fill(std::string &directive,
             std::string &directive_uppercase,
             std::string &t_file,
             size_t line_number,
             size_t column);
        Fill(std::string &&directive,
             std::string &&directive_uppercase,
             std::string &t_file,
             size_t line_number,
             size_t column);

        virtual Token::token_type type() const override;

        virtual std::int32_t assemble(std::vector<std::shared_ptr<Token>> &tokens,
                                      const std::map<std::string, Symbol> &symbols,
                                      uint16_t program_counter) override;
        virtual std::int32_t guess_memory_size(std::vector<std::shared_ptr<Token>> &tokens) const override;

        virtual bool valid_arguments(std::vector<std::shared_ptr<Token>> &tokens) override;

        virtual std::string disassemble(
                uint16_t &program_counter,
                const std::string &symbol,
                int width) const override;
};

#endif // TOKEN_DIRECTIVE_FILL_HPP
