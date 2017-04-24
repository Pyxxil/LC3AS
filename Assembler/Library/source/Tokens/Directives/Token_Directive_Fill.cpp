#include "Tokens/Directives/Token_Directive_Fill.hpp"

#include <algorithm>

#include "Tokens/Token_Immediate.hpp"
#include "Tokens/Token_Label.hpp"
#include "Assembler.hpp"

Fill::Fill(std::string &token, int line_number)
        : Directive(token, line_number)
{}

std::int32_t Fill::assemble(std::vector<std::shared_ptr<Token>> &tokens, Assembler &assembler)
{
        if (!is_valid) {
                return 0;
        }

        if (tokens.at(1)->type() == Token::IMMEDIATE) {
                assembled.emplace_back(static_cast<std::uint16_t>(std::static_pointer_cast<
                        Immediate>(tokens.at(1))->immediate));
        } else if (tokens.at(1)->type() == Token::LABEL) {
                const auto &&symbol = std::find_if(assembler.symbols.cbegin(),
                                                   assembler.symbols.cend(),
                                                   [&tokens](auto &&sym) -> bool
                                                   {
                                                           return sym.second->word == tokens.at(1)->word;
                                                   });

                if (symbol == assembler.symbols.end()) {
                        std::static_pointer_cast<Label>(tokens.at(1))->not_found();
                        return -1;
                } else {
                        assembled.emplace_back(symbol->second->address);
                }
        }

        return 1;
}

bool Fill::valid_arguments(std::vector<std::shared_ptr<Token>> &tokens)
{
        if (tokens.size() < 2) {
                invalid_argument_count(tokens.size(), 1);
                return (is_valid = false);
        }

        if (tokens.at(1)->type() != Token::IMMEDIATE && tokens.at(1)->type() != Token::LABEL) {
                tokens.at(1)->expected("register or label");
                return (is_valid = false);
        } else if (!tokens.at(1)->is_valid) {
                return (is_valid = false);
        }

        return is_valid;
}

std::int32_t Fill::guess_memory_size(std::vector<std::shared_ptr<Token>> &tokens) const
{
        (void) tokens;
        return static_cast<std::int32_t>(is_valid);
}

Token::token_type Fill::type() const
{
        return Token::token_type::DIR_FILL;
}
