#include "Tokens/Additional/Token_Addon_Neg.hpp"

#include <iomanip>
#include <sstream>

#include "Tokens/Immediate/Token_Immediate_Decimal.hpp"
#include "Tokens/Token_Register.hpp"
#include "Assembler.hpp"

Neg::Neg()
        : Directive()
{
        neg = std::make_shared<Not>();
        add = std::make_shared<Add>();
}

Neg::Neg(std::string &directive, std::string &token_uppercase, int line_number)
        : Directive(directive, token_uppercase, line_number)
{
        neg = std::make_shared<Not>();
        add = std::make_shared<Add>();

        neg->at_line = add->at_line = line_number;
}

std::int32_t Neg::assemble(std::vector<std::shared_ptr<Token>> &tokens, Assembler &assembler)
{
        std::vector<std::shared_ptr<Token>> vec = {neg, tokens.at(1), tokens.at(1)};

        neg->assemble(vec, assembler);
        vec = {add, tokens.at(1), tokens.at(1), std::make_shared<Decimal>("#1")};
        add->assemble(vec, assembler);

        assembled = neg->assembled;
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

std::string Neg::disassemble(std::vector<std::shared_ptr<Token>> &tokens,
                             std::uint16_t &program_counter,
                             const std::string &symbol,
                             const Assembler &assembler) const
{
        std::stringstream stream;

        std::vector<std::shared_ptr<Token>> vec = {neg, tokens.at(1), tokens.at(1)};
        stream << neg->disassemble(vec, program_counter, symbol, assembler);

        vec = {add, tokens.at(1), tokens.at(1), std::make_shared<Decimal>("#1")};
        stream << add->disassemble(vec, program_counter, " ", assembler);

        return stream.str();
}

Token::token_type Neg::type() const
{
        return Token::ADDON_NEG;
}
