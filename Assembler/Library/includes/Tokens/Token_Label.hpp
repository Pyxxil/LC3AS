#ifndef PROJECT_TOKEN_LABEL_HPP
#define PROJECT_TOKEN_LABEL_HPP

#include "../Token.hpp"

class Label : public Token
{
        // TODO: Add some sort of fuzzy match to check if the user meant something else
public:
        Label(std::string &label, int line_number);

        virtual Token::token_type type() const override;

        virtual std::int32_t assemble(std::vector<std::shared_ptr<Token>> &tokens, Assembler &assembler) override;
        virtual std::int32_t guess_memory_size(std::vector<std::shared_ptr<Token>> &tokens) const override;

        virtual bool valid_arguments(std::vector<std::shared_ptr<Token>> &tokens) override;

        void not_found();

        virtual std::string disassemble(std::vector<std::shared_ptr<Token>> &tokens,
                                        std::uint16_t &program_counter,
                                        const std::string &symbol,
                                        const Assembler &assembler) const override;

        std::uint16_t address = 0;
};

#endif //PROJECT_TOKEN_LABEL_HPP
