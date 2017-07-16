#include "Tokens/Token_Label.hpp"

#include "String_Matcher.hpp"
#include "Diagnostics.hpp"
#include "LexHelper.hpp"

Label::Label(std::string &name, std::string &t_file, size_t line_number, size_t column)
        : Token(name, name, t_file, line_number, column)
{

}

std::int32_t Label::assemble(std::vector<std::shared_ptr<Token>> &tokens,
                             const std::map<std::string, Symbol> &symbols,
                             uint16_t program_counter)
{
        if (!is_valid) {
                return -1;
        }

        if (tokens.size() == 1) {
                return 0;
        }

        std::vector<std::shared_ptr<Token>> vec(tokens.begin() + 1, tokens.end());

        const std::int32_t ret = vec.front()->assemble(vec, symbols, program_counter);

        assembled = vec.front()->assembled;
        return ret;
}

bool Label::valid_arguments(std::vector<std::shared_ptr<Token>> &tokens)
{
        if (tokens.size() > 1) {
                instruction = tokens.at(1);
                std::vector<std::shared_ptr<Token>> vec(tokens.begin() + 1, tokens.end());
                return instruction->valid_arguments(vec);
        }

        return true;
}

std::int32_t Label::guess_memory_size(std::vector<std::shared_ptr<Token>> &tokens) const
{
        if (tokens.size() > 1) {
                std::vector<std::shared_ptr<Token>> vec(tokens.begin() + 1, tokens.end());
                return vec.front()->guess_memory_size(vec);
        }

        return 0;
}

void Label::not_found(const std::map<std::string, Symbol> &match_candidates)
{
        String_Matcher matcher(token);

        for (const auto &symbol : match_candidates) {
                matcher.consider(symbol.first);
        }

        auto &&possible_match = matcher.best_match();

        Diagnostics::Diagnostic diag(
                Diagnostics::FileContext(
                        Diagnostics::Variant<std::string>(file, Console::Colour(Console::FOREGROUND_COLOUR::YELLOW)),
                        Diagnostics::Variant<size_t>(at_line, Console::Colour(Console::FOREGROUND_COLOUR::YELLOW)),
                        Diagnostics::Variant<size_t>(at_column, Console::Colour(Console::FOREGROUND_COLOUR::YELLOW))
                ), "Invalid label", Diagnostics::INVALID_LABEL, Diagnostics::ERROR
        );

        if (!possible_match.empty()) {
                auto &&sym = match_candidates.find(possible_match);
                diag.provide_context(
                        std::make_unique<Diagnostics::HighlightContext>(
                                Diagnostics::SelectionContext(
                                        Diagnostics::FileContext(
                                                Diagnostics::Variant<std::string>(
                                                        file,
                                                        Console::Colour(Console::FOREGROUND_COLOUR::YELLOW)
                                                ),
                                                Diagnostics::Variant<size_t>(
                                                        at_line,
                                                        Console::Colour(Console::FOREGROUND_COLOUR::YELLOW)
                                                ),
                                                Diagnostics::Variant<size_t>(
                                                        at_column,
                                                        Console::Colour(Console::FOREGROUND_COLOUR::YELLOW)
                                                )
                                        ), '^', "No such label '" + token + "'; Did you mean '" + possible_match + "'?",
                                        lexed_lines[file].at(at_line - 1)
                                ), '~', token.size(), possible_match
                        )
                );

                // TODO: Fix match candidates to provide file, and column.
                diag.provide_context(
                        std::make_unique<Diagnostics::HighlightContext>(
                                Diagnostics::SelectionContext(
                                        Diagnostics::FileContext(
                                                Diagnostics::Variant<std::string>(
                                                        sym->second.file,
                                                        Console::Colour(Console::FOREGROUND_COLOUR::YELLOW)
                                                ),
                                                Diagnostics::Variant<size_t>(
                                                        sym->second.line_number,
                                                        Console::Colour(Console::FOREGROUND_COLOUR::YELLOW)
                                                ),
                                                Diagnostics::Variant<size_t>(
                                                        sym->second.column,
                                                        Console::Colour(Console::FOREGROUND_COLOUR::YELLOW)
                                                )
                                        ), '^', "'" + possible_match + "' declared here",
                                        lexed_lines[sym->second.file].at(sym->second.line_number - 1)
                                ),
                                '~', sym->first.length()
                        )
                );
        }

        Diagnostics::push(diag);
}

std::string Label::disassemble(uint16_t &program_counter,
                               const std::string &symbol,
                               int width) const
{
        if (!assembled.empty()) {
                return instruction->disassemble(program_counter, symbol, width);
        }

        return "";
}

void Label::requires_too_many_bits(int allowed_bits,
                                   bool is_signed,
                                   const Token *const caller,
                                   const std::map<std::string, Symbol> &symbols)
{
        (void) caller;

        Diagnostics::Diagnostic diag(
                Diagnostics::FileContext(
                        Diagnostics::Variant<std::string>(file, Console::Colour(Console::FOREGROUND_COLOUR::YELLOW)),
                        Diagnostics::Variant<size_t>(at_line, Console::Colour(Console::FOREGROUND_COLOUR::YELLOW)),
                        Diagnostics::Variant<size_t>(at_column, Console::Colour(Console::FOREGROUND_COLOUR::YELLOW))
                ), "Address too far away.", Diagnostics::INVALID_LABEL, Diagnostics::ERROR
        );

        std::stringstream ss;
        ss << "Address of '" << token << "' requires offset which can't be represented in a "
           << allowed_bits << " bit " << (is_signed ? "signed" : "unsigned") << " PC offset.";

        diag.provide_context(
                std::make_unique<Diagnostics::HighlightContext>(
                        Diagnostics::SelectionContext(
                                Diagnostics::FileContext(
                                        Diagnostics::Variant<std::string>(
                                                file, Console::Colour(Console::FOREGROUND_COLOUR::YELLOW)
                                        ),
                                        Diagnostics::Variant<size_t>(
                                                at_line, Console::Colour(Console::FOREGROUND_COLOUR::YELLOW)
                                        ),
                                        Diagnostics::Variant<size_t>(
                                                at_column, Console::Colour(Console::FOREGROUND_COLOUR::YELLOW)
                                        )
                                ), '^', ss.str(), lexed_lines[file].at(at_line - 1)
                        ), '~', token.length()
                )
        );

        auto &&sym = symbols.find(token);
        diag.provide_context(
                std::make_unique<Diagnostics::HighlightContext>(
                        Diagnostics::SelectionContext(
                                Diagnostics::FileContext(
                                        Diagnostics::Variant<std::string>(
                                                sym->second.file,
                                                Console::Colour(Console::FOREGROUND_COLOUR::YELLOW)
                                        ),
                                        Diagnostics::Variant<size_t>(
                                                sym->second.line_number,
                                                Console::Colour(Console::FOREGROUND_COLOUR::YELLOW)
                                        ),
                                        Diagnostics::Variant<size_t>(
                                                sym->second.column,
                                                Console::Colour(Console::FOREGROUND_COLOUR::YELLOW)
                                        )
                                ), '^', "'" + sym->first + "' declared here",
                                lexed_lines[sym->second.file].at(sym->second.line_number - 1)
                        ), '~', sym->first.length()
                )
        );

        Diagnostics::push(diag);
}

Token::token_type Label::type() const
{
        return Token::token_type::LABEL;
}
