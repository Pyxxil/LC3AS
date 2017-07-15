#include "Parser.hpp"

#include "Tokens/Tokens.hpp"
#include "Diagnostics.hpp"
#include "LexHelper.hpp"
#include "Lexer.hpp"

void Parser::do_first_pass()
{
        int memory_required = 0;

        // TODO: As it is, if there is no .ORIG directive as the first instruction in the file,
        // TODO: we still look at the rest of the file. Is it smarter to stop if we don't see it
        // TODO: first? Or should we just default to setting the memory address to 0x3000?

        const auto &memory_requirement_of = [this](const auto &token, auto &&t_tokens) -> std::int32_t
        {
                if (!origin_seen) {
                        token->expected(".ORIG statement");
                        return -1;
                } else if (end_seen) {
                        Diagnostics::push(
                                Diagnostics::Diagnostic(
                                        Diagnostics::FileContext(
                                                Diagnostics::Variant<std::string>(token->file, Console::FOREGROUND_COLOUR::YELLOW),
                                                Diagnostics::Variant<size_t>(token->at_line,
                                                                                  Console::FOREGROUND_COLOUR::YELLOW),
                                                Diagnostics::Variant<size_t>(token->at_column,
                                                                                  Console::FOREGROUND_COLOUR::YELLOW)
                                        ),
                                        "Token after .END directive, it will be ignored.", Diagnostics::SYNTAX, Diagnostics::WARNING
                                )
                        );
                        return 0;
                } else if (token->valid_arguments(t_tokens)) {
                        return token->guess_memory_size(t_tokens);
                } else {
                        return -1;
                }
        };

        for (auto &&tokenized_line : tokens) {
                switch (tokenized_line.front()->type()) {
                case Token::DIR_ORIG:
                        if (origin_seen) {
                                Diagnostics::Diagnostic diag(
                                        Diagnostics::FileContext(
                                                Diagnostics::Variant<std::string>(tokenized_line.front()->file, Console::FOREGROUND_COLOUR::YELLOW),
                                                Diagnostics::Variant<size_t>(tokenized_line.front()->at_line,
                                                                                  Console::FOREGROUND_COLOUR::YELLOW),
                                                Diagnostics::Variant<size_t>(tokenized_line.front()->at_column,
                                                                                  Console::FOREGROUND_COLOUR::YELLOW)
                                        ),
                                        "Redefinition of Origin memory address.", Diagnostics::MULTIPLE_DEFINITIONS, Diagnostics::WARNING
                                );

                                // TODO: Do we try to find the original definition?

                                Diagnostics::push(diag);
                                break;
                        }
                        origin_seen     = true;
                        memory_required = memory_requirement_of(tokenized_line.front(), tokenized_line);
                        if (memory_required > -1) {
                                internal_program_counter += memory_required;
                        }
                        break;
                case Token::LABEL:
                        memory_required = memory_requirement_of(tokenized_line.front(), tokenized_line);
                        if (memory_required > -1) {
                                std::static_pointer_cast<Label>(tokenized_line.front())->address =
                                        internal_program_counter;

                                for (const auto &symbol : symbols) {
                                        if (symbol.second.address == internal_program_counter) {
                                                Diagnostics::Diagnostic diag(
                                                        Diagnostics::FileContext(
                                                                Diagnostics::Variant<std::string>(tokenized_line.front()->file, Console::FOREGROUND_COLOUR::YELLOW),
                                                                Diagnostics::Variant<size_t>(tokenized_line.front()->at_line,
                                                                                                  Console::FOREGROUND_COLOUR::YELLOW),
                                                                Diagnostics::Variant<size_t>(tokenized_line.front()->at_column, Console::FOREGROUND_COLOUR::YELLOW)
                                                        ),
                                                        "Multiple labels found for address", Diagnostics::MULTIPLE_DEFINITIONS, Diagnostics::ERROR
                                                );

                                                diag.provide_context(
                                                        std::make_unique<Diagnostics::SelectionContext>(
                                                                Diagnostics::FileContext(
                                                                        Diagnostics::Variant<std::string>(symbol.second.file, Console::FOREGROUND_COLOUR::YELLOW),
                                                                        Diagnostics::Variant<size_t>(symbol.second.line_number,
                                                                                                          Console::FOREGROUND_COLOUR::YELLOW),
                                                                        Diagnostics::Variant<size_t>(symbol.second.column,
                                                                                                          Console::FOREGROUND_COLOUR::YELLOW)
                                                                ), '^', "Previous label found here",
                                                                std::string(lexed_lines[symbol.second.file].at(symbol.second.line_number - 1))
                                                        )
                                                );

                                                Diagnostics::push(diag);
                                                break;
                                        }
                                }

                                if (symbols.count(tokenized_line.front()->token)) {
                                        auto &&sym = symbols.at(tokenized_line.front()->token);
                                        Diagnostics::Diagnostic diag(
                                                Diagnostics::FileContext(
                                                        Diagnostics::Variant<std::string>(tokenized_line.front()->file, Console::FOREGROUND_COLOUR::YELLOW),
                                                        Diagnostics::Variant<size_t>(tokenized_line.front()->at_line,
                                                                                          Console::FOREGROUND_COLOUR::YELLOW),
                                                        Diagnostics::Variant<size_t>(tokenized_line.front()->at_column,
                                                                                          Console::FOREGROUND_COLOUR::YELLOW)
                                                ),
                                                "Multiple definitions of label", Diagnostics::MULTIPLE_DEFINITIONS, Diagnostics::WARNING
                                        );

                                        diag.provide_context(
                                                std::make_unique<Diagnostics::SelectionContext>(
                                                        Diagnostics::FileContext(
                                                                Diagnostics::Variant<std::string>(tokenized_line.front()->file, Console::FOREGROUND_COLOUR::YELLOW),
                                                                Diagnostics::Variant<size_t>(tokenized_line.front()->at_line,
                                                                                                  Console::FOREGROUND_COLOUR::YELLOW),
                                                                Diagnostics::Variant<size_t>(tokenized_line.front()->at_column,
                                                                                                  Console::FOREGROUND_COLOUR::YELLOW)
                                                        ), '^', "Redefinition of this label",
                                                        std::string(lexed_lines[tokenized_line.front()->file].at(tokenized_line.front()->at_line - 1))
                                                )
                                        );

                                        diag.provide_context(
                                                std::make_unique<Diagnostics::SelectionContext>(
                                                        Diagnostics::FileContext(
                                                                Diagnostics::Variant<std::string>(sym.file, Console::FOREGROUND_COLOUR::YELLOW),
                                                                Diagnostics::Variant<size_t>(sym.line_number,
                                                                                                  Console::FOREGROUND_COLOUR::YELLOW),
                                                                Diagnostics::Variant<size_t>(sym.column,
                                                                                                  Console::FOREGROUND_COLOUR::YELLOW)
                                                        ), '^', "Previous definition found here", std::string(lexed_lines[sym.file].at(sym.line_number - 1))
                                                )
                                        );

                                        Diagnostics::push(diag);
                                }

                                symbols.insert(std::pair<std::string, Symbol>(
                                        tokenized_line.front()->token,
                                        Symbol(internal_program_counter, tokenized_line.front()->at_line,
                                               tokenized_line.front()->at_column, tokenized_line.front()->file))
                                );

                                longest_symbol_length = std::max(
                                        longest_symbol_length,
                                        static_cast<int>(tokenized_line.front()->token.length())
                                );

                                internal_program_counter += memory_required;
                        }
                        break;
                case Token::DIR_END:
                        end_seen = true;
                        break;
                default:
                        memory_required = memory_requirement_of(tokenized_line.front(), tokenized_line);
                        if (memory_required > -1) {
                                internal_program_counter += memory_required;
                        }
                        break;
                }
        }
}

void Parser::do_second_pass()
{
        int memory_required = 0;

        for (auto &&tokenized_line : tokens) {
                if (tokenized_line.front()->type() == Token::DIR_END) {
                        end_seen = true;
                        break;
                }

                memory_required = tokenized_line.front()->assemble(tokenized_line, symbols, internal_program_counter);

                if (memory_required >= 0) {
                        internal_program_counter += static_cast<uint16_t>(memory_required);
                }
        }
}

int Parser::parse()
{
        Lexer lexer(file);
        lexer.lex(tokens, symbols);

        if (Diagnostics::critical()) {
                // There were errors, so don't bother continuing.
                return 1;
        }

        do_first_pass();

        if (Diagnostics::critical()) {
                return 1;
        }

        origin_seen = false;
        end_seen    = false;

        program_counter = internal_program_counter;
        internal_program_counter = 0;

        do_second_pass();

        // Tell the caller whether there are no errors (so, continue with whatever), or there were errors on the second pass.
        return Diagnostics::critical();
}
