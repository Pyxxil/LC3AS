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

Neg::Neg(std::string &word, int line_number)
        : Token(word, line_number)
{

}

std::int32_t Neg::assemble(std::vector<std::shared_ptr<Token>> &tokens, Assembler &assembler)
{
        if (assembled.size()) {
                return static_cast<std::int32_t>(assembled.size());
        }

        if (tokens.size() != 2) {
                invalid_argument_count(tokens.size(), 1);
                return -1;
        }

        if (!assembler.origin_seen) {
                expected(".ORIG directive");
                return -1;
        } else if (assembler.end_seen) {
                std::cerr << "WARNING: ";
                if (at_line) {
                        std::cerr << "Line " << std::dec << at_line << ": ";
                }
                std::cerr << "NOT after .END directive. It will be ignored.\n";
                return 0;
        }

        if (tokens[1]->type() != Token::REGISTER) {
                tokens[1]->expected("register");
                return -1;
        }

        if (tokens[1]->is_error) {
                return 0;
        }

        std::shared_ptr<Token>              neg = std::make_shared<Not>();
        std::vector<std::shared_ptr<Token>> vec = {neg, tokens[1], tokens[1]};
        neg->assemble(vec, assembler);

        std::shared_ptr<Token> add = std::make_shared<Add>();
        vec = {add, tokens[1], tokens[1], std::make_shared<Decimal>("#1")};
        add->assemble(vec, assembler);

        assembled.swap(neg->assembled);
        for (auto &&as_assembled : add->assembled) {
                assembled.emplace_back(as_assembled);
        }

        return 2;
}

Token::token_type Neg::type() const
{
        return Token::ADDON_NEG;
}
