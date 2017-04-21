#include "Tokens/Additional/Token_Addon_Neg.hpp"

#include "Tokens/Instructions/Token_Instruction_Not.hpp"
#include "Tokens/Instructions/Token_Instruction_Add.hpp"
#include "Tokens/Immediate/Token_Immediate_Decimal.hpp"
#include "Tokens/Token_Register.hpp"
#include "Assembler.hpp"

Neg::Neg()
        : Token()
{

}

Neg::Neg(std::string &directive, int line_number)
        : Token(directive, line_number)
{

}

std::int32_t Neg::assemble(std::vector<std::shared_ptr<Token>> &tokens, Assembler &assembler)
{
        std::shared_ptr<Token>              neg = std::make_shared<Not>();
        std::vector<std::shared_ptr<Token>> vec = {neg, tokens[1], tokens[1]};
        neg->assemble(vec, assembler);

        std::shared_ptr<Token> add = std::make_shared<Add>();
        vec = {add, tokens[1], tokens[1], std::make_shared<Decimal>("#1")};
        add->assemble(vec, assembler);

        assembled.swap(neg->assembled);
        for (const auto &as_assembled : add->assembled) {
                assembled.emplace_back(as_assembled);
        }

        return 2;
}

bool Neg::valid_arguments(std::vector<std::shared_ptr<Token>> &tokens)
{
        if (tokens.size() != 2) {
                invalid_argument_count(tokens.size(), 1);
                return (is_valid = false);
        }

        if (tokens.at(1)->type() != Token::REGISTER) {
                tokens.at(1)->expected("register");
                return (is_valid = false);
        }

        return is_valid;
}

std::int32_t Neg::guess_memory_size(std::vector<std::shared_ptr<Token>> &tokens) const
{
        (void) tokens;
        return static_cast<std::int32_t>(is_valid) * 2;
}

Token::token_type Neg::type() const
{
        return Token::ADDON_NEG;
}
