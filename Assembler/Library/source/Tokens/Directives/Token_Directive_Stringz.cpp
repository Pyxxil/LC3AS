#include "Tokens/Directives/Token_Directive_Stringz.hpp"

#include <Assembler.hpp>

Stringz::Stringz(std::string &token, int line_number)
        : Directive(token, line_number)
{}

std::int32_t Stringz::assemble(std::vector<std::shared_ptr<Token>> &tokens, Assembler &assembler)
{
        (void) assembler;
        if (!is_valid) {
                return -1;
        }

        assembled = tokens.at(1)->assembled;
        return static_cast<std::int32_t>(assembled.size());
}

bool Stringz::valid_arguments(std::vector<std::shared_ptr<Token>> &tokens)
{
        if (tokens.size() != 2) {
                invalid_argument_count(tokens.size(), 1);
                return (is_valid = false);
        }

        if (tokens.at(1)->type() != Token::_STRING) {
                tokens.at(1)->expected("string literal");
                return (is_valid = false);
        } else if (!tokens.at(1)->is_valid) {
                return (is_valid = false);
        }

        return is_valid;
}

std::int32_t Stringz::guess_memory_size(std::vector<std::shared_ptr<Token>> &tokens) const
{
        return static_cast<std::int32_t>(is_valid) * static_cast<std::int32_t>(tokens.at(1)->assembled.size());
}

Token::token_type Stringz::type() const
{
        return Token::token_type::DIR_STRINGZ;
}
