#ifndef PROJECT_TOKEN_ADDON_NEG_HPP
#define PROJECT_TOKEN_ADDON_NEG_HPP

#include <Token.hpp>

class Neg : public Token
{
public:
        Neg();
        Neg(std::string &word, int line_number = 0);
        virtual int32_t assemble(std::vector<std::shared_ptr<Token>> &tokens, Assembler &assembler) override;
        virtual token_type type() const override;
        virtual bool valid_arguments(std::vector<std::shared_ptr<Token>> &tokens) override;
        virtual int32_t guess_memory_size(std::vector<std::shared_ptr<Token>> &tokens) const override;
};

#endif //PROJECT_TOKEN_ADDON_NEG_HPP
