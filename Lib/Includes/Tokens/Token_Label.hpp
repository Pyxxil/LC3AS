#ifndef TOKEN_LABEL_HPP
#define TOKEN_LABEL_HPP

#include "Tokens/Token.hpp"

class Label : public Token
{
public:
        Label(std::string &label, std::string &t_file, size_t line_number, size_t column);

        virtual Token::token_type type() const override;

        virtual std::int32_t assemble(std::vector<std::shared_ptr<Token>> &tokens,
                                      const std::map<std::string, Symbol> &symbols,
                                      uint16_t program_counter) override;
        virtual std::int32_t guess_memory_size(std::vector<std::shared_ptr<Token>> &tokens) const override;

        virtual bool valid_arguments(std::vector<std::shared_ptr<Token>> &tokens) override;

        void not_found(const std::map<std::string, Symbol> &match_candidates);
        virtual void requires_too_many_bits(int allowed_bits,
                                            bool is_signed,
                                            const Token *const caller,
                                            const std::map<std::string, Symbol> &symbols) override;

        virtual std::string disassemble(uint16_t &program_counter,
                                        const std::string &symbol,
                                        int width) const override;

        uint16_t address = 0;

private:
        std::shared_ptr<Token> instruction;
};

#endif // TOKEN_LABEL_HPP
