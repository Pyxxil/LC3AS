#ifndef TOKEN_DIRECTIVE_BLKW_HPP
#define TOKEN_DIRECTIVE_BLKW_HPP

#include "Tokens/Token_Directive.hpp"
#include "Tokens/Token_Immediate.hpp"

class Blkw : public Directive
{
public:
        Blkw(std::string &directive, std::string &directive_uppercase, int line_number = 0);

        virtual Token::token_type type() const override;

        virtual std::int32_t assemble(std::vector<std::shared_ptr<Token>> &tokens, const Assembler &assembler) override;
        virtual std::int32_t guess_memory_size(std::vector<std::shared_ptr<Token>> &tokens) const override;

        virtual void invalid_argument_count(std::size_t provided, std::size_t expected) const override;

        virtual bool valid_arguments(std::vector<std::shared_ptr<Token>> &tokens) override;

        virtual std::string disassemble(std::vector<std::shared_ptr<Token>> &tokens,
                                        std::uint16_t &program_counter,
                                        const std::string &symbol,
                                        const Assembler &assembler) const override;
};

#endif // TOKEN_DIRECTIVE_BLKW_HPP
