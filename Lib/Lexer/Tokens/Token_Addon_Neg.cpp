#include "Tokens/Token_Addon_Neg.hpp"

#include <iomanip>
#include <sstream>

#include "Tokens/Token_Immediate_Decimal.hpp"

#ifdef INCLUDE_ADDONS

Neg::Neg(std::string &directive,
         std::string &directive_uppercase,
         std::string &t_file,
         size_t line_number,
         size_t column)
        : Directive(directive, directive_uppercase, t_file, line_number, column)
          , neg(std::make_shared<Not>("NOT", "NOT", t_file, line_number, column))
          , add(std::make_shared<Add>("ADD", "ADD", t_file, line_number, column))
{

}

Neg::Neg(std::string &&directive,
         std::string &&directive_uppercase,
         std::string &t_file,
         size_t line_number,
         size_t column)
        : Neg(directive, directive_uppercase, t_file, line_number, column)
{

}

std::int32_t Neg::assemble(std::vector<std::shared_ptr<Token>> &tokens,
                           const std::map<std::string, Symbol> &symbols,
                           uint16_t program_counter)
{
        std::vector<std::shared_ptr<Token>> vec = { neg, tokens.at(1), tokens.at(1) };

        neg->assemble(vec, symbols, program_counter);
        vec = { add, tokens.at(1), tokens.at(1), std::make_shared<Decimal>("#1", file, at_line, at_column) };
        add->assemble(vec, symbols, program_counter);

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

std::string Neg::disassemble(uint16_t &program_counter,
                             const std::string &symbol,
                             int width) const
{
        std::stringstream stream;

        stream << neg->disassemble(program_counter, symbol, width);
        stream << add->disassemble(program_counter, " ", width);

        return stream.str();
}

Token::token_type Neg::type() const
{
        return Token::ADDON_NEG;
}

#endif