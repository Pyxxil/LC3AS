#ifndef PROJECT_TOKEN_DIRECTIVE_ORIG_HPP
#define PROJECT_TOKEN_DIRECTIVE_ORIG_HPP

#include "Tokens/Token_Directive.hpp"
#include "Tokens/Token_Immediate.hpp"

class Orig : public Directive
{
public:
        Orig(std::string &word, int line_number = 0);

        std::uint16_t origin = 0x3000;

        virtual Token::token_type type() const override;

        std::int32_t assemble(std::vector<std::shared_ptr<Token>> &tokens, Assembler &assembler) override;
};

#endif //PROJECT_TOKEN_DIRECTIVE_ORIG_HPP
