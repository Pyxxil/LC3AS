#ifndef TOKEN_LABEL_HPP
#define TOKEN_LABEL_HPP

#include "../Token.hpp"

class Label : public Token
{
public:
        Label(std::string &label, int line_number);

        virtual Token::token_type type() const override;

        virtual std::int32_t assemble(std::vector<std::shared_ptr<Token>> &tokens, const Assembler &assembler) override;
        virtual std::int32_t guess_memory_size(std::vector<std::shared_ptr<Token>> &tokens) const override;

        virtual bool valid_arguments(std::vector<std::shared_ptr<Token>> &tokens) override;

        void not_found(const std::string &match_candidate);
        virtual void requires_too_many_bits(int allowed_bits, bool is_signed = true) override;

        virtual std::string disassemble(std::vector<std::shared_ptr<Token>> &tokens,
                                        std::uint16_t &program_counter,
                                        const std::string &symbol,
                                        const Assembler &assembler) const override;

        std::uint16_t address = 0;
};

#endif // TOKEN_LABEL_HPP
