#include "Parser.hpp"

#include "Tokens/Tokens.hpp"
#include "Configuration.hpp"
#include "Diagnostics.hpp"
#include "LexHelper.hpp"
#include "Lexer.hpp"

/*! Generate the symbol table
 *
 * Create the symbol table, given a starting address by a .ORIG directive. If none is found, this
 * currently treats that as an error (the same happens if an instruction of some kind is found before
 * a .ORIG statement).
 *
 * The position in memory for each instruction is dependent on our best guess as to how much memory
 * each instruction between each label.
 */
void Parser::do_first_pass()
{
        int memory_required = 0;

        /**
         * Make our best guess at how much memory a line requires.
         *
         * @param token: The token currently being processed.
         * @param t_tokens: All of the tokens on that line (needed for calling guess_memory_size on token)
         *
         * @returns The best guess we have for how much memory the line requires.
         */
        const auto &memory_requirement_of = [this](const auto &token, auto &&t_tokens) -> std::int32_t
        {
                if (!origin_seen) {
                        token->expected(".ORIG statement");
                        return -1;
                }

                if (end_seen) {
                        // TODO: Do we show the user where the .END directive was?
                        Diagnostics::push(
                                Diagnostics::Diagnostic(
                                        Diagnostics::FileContext(
                                                Diagnostics::Variant<std::string>(
                                                        token->file,
                                                        Console::Colour(Console::FOREGROUND_COLOUR::YELLOW)
                                                ),
                                                Diagnostics::Variant<size_t>(
                                                        token->at_line,
                                                        Console::Colour(Console::FOREGROUND_COLOUR::YELLOW)
                                                ),
                                                Diagnostics::Variant<size_t>(
                                                        token->at_column,
                                                        Console::Colour(Console::FOREGROUND_COLOUR::YELLOW)
                                                )
                                        ),
                                        "Token after .END directive, it will be ignored.",
                                        Diagnostics::SYNTAX,
                                        Diagnostics::WARNING
                                )
                        );
                        return 0;
                }

                if (token->valid_arguments(t_tokens)) {
                        return token->guess_memory_size(t_tokens);
                }

                return -1;
        };

        // TODO: As it is, if there is no .ORIG directive as the first instruction in the file,
        // TODO: we still look at the rest of the file. Is it smarter to stop if we don't see it
        // TODO: first? Or should we just default to setting the memory address to 0x3000?
        for (auto &&tokenized_line : tokens) {
                switch (tokenized_line.front()->type()) {
                case Token::DIR_ORIG:
                        if (origin_seen) {
                                Diagnostics::Diagnostic diag(
                                        Diagnostics::FileContext(
                                                Diagnostics::Variant<std::string>(
                                                        tokenized_line.front()->file,
                                                        Console::Colour(Console::FOREGROUND_COLOUR::YELLOW)
                                                ),
                                                Diagnostics::Variant<size_t>(
                                                        tokenized_line.front()->at_line,
                                                        Console::Colour(Console::FOREGROUND_COLOUR::YELLOW)
                                                ),
                                                Diagnostics::Variant<size_t>(
                                                        tokenized_line.front()->at_column,
                                                        Console::Colour(Console::FOREGROUND_COLOUR::YELLOW)
                                                )
                                        ),
                                        "Redefinition of Origin memory address.",
                                        Diagnostics::MULTIPLE_DEFINITIONS,
                                        Diagnostics::WARNING
                                );

                                // TODO: Do we try to find the original definition?

                                Diagnostics::push(diag);
                                break;
                        }
                        origin_seen     = true;
                        memory_required = memory_requirement_of(tokenized_line.front(), tokenized_line);
                        if (memory_required > -1) {
                                internal_program_counter += static_cast<uint16_t>(memory_required);
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
                                                                Diagnostics::Variant<std::string>(
                                                                        tokenized_line.front()->file,
                                                                        Console::Colour(Console::FOREGROUND_COLOUR::YELLOW)
                                                                ),
                                                                Diagnostics::Variant<size_t>(
                                                                        tokenized_line.front()->at_line,
                                                                        Console::Colour(Console::FOREGROUND_COLOUR::YELLOW)
                                                                ),
                                                                Diagnostics::Variant<size_t>(
                                                                        tokenized_line.front()->at_column,
                                                                        Console::Colour(Console::FOREGROUND_COLOUR::YELLOW)
                                                                )
                                                        ),
                                                        "Multiple labels found for address",
                                                        Diagnostics::MULTIPLE_DEFINITIONS,
                                                        Diagnostics::ERROR
                                                );

                                                diag.provide_context(
                                                        std::make_unique<Diagnostics::SelectionContext>(
                                                                Diagnostics::FileContext(
                                                                        Diagnostics::Variant<std::string>(
                                                                                symbol.second.file,
                                                                                Console::Colour(Console::FOREGROUND_COLOUR::YELLOW)
                                                                        ),
                                                                        Diagnostics::Variant<size_t>(
                                                                                symbol.second.line_number,
                                                                                Console::Colour(Console::FOREGROUND_COLOUR::YELLOW)
                                                                        ),
                                                                        Diagnostics::Variant<size_t>(
                                                                                symbol.second.column,
                                                                                Console::Colour(Console::FOREGROUND_COLOUR::YELLOW)
                                                                        )
                                                                ), '^', "Previous label found here",
                                                                lexed_lines[symbol.second.file].at(
                                                                        symbol.second.line_number - 1
                                                                )
                                                        )
                                                );

                                                Diagnostics::push(diag);
                                                break;
                                        }
                                }

                                if (0u != symbols.count(tokenized_line.front()->token)) {
                                        auto                    &&sym = symbols.at(tokenized_line.front()->token);
                                        Diagnostics::Diagnostic diag(
                                                Diagnostics::FileContext(
                                                        Diagnostics::Variant<std::string>(
                                                                tokenized_line.front()->file,
                                                                Console::Colour(Console::FOREGROUND_COLOUR::YELLOW)
                                                        ),
                                                        Diagnostics::Variant<size_t>(
                                                                tokenized_line.front()->at_line,
                                                                Console::Colour(Console::FOREGROUND_COLOUR::YELLOW)
                                                        ),
                                                        Diagnostics::Variant<size_t>(
                                                                tokenized_line.front()->at_column,
                                                                Console::Colour(Console::FOREGROUND_COLOUR::YELLOW)
                                                        )
                                                ),
                                                "Multiple definitions of label",
                                                Diagnostics::MULTIPLE_DEFINITIONS,
                                                Diagnostics::ERROR
                                        );

                                        diag.provide_context(
                                                std::make_unique<Diagnostics::SelectionContext>(
                                                        Diagnostics::FileContext(
                                                                Diagnostics::Variant<std::string>(
                                                                        tokenized_line.front()->file,
                                                                        Console::Colour(Console::FOREGROUND_COLOUR::YELLOW)
                                                                ),
                                                                Diagnostics::Variant<size_t>(
                                                                        tokenized_line.front()->at_line,
                                                                        Console::Colour(Console::FOREGROUND_COLOUR::YELLOW)
                                                                ),
                                                                Diagnostics::Variant<size_t>(
                                                                        tokenized_line.front()->at_column,
                                                                        Console::Colour(Console::FOREGROUND_COLOUR::YELLOW)
                                                                )
                                                        ), '^', "Redefinition of this label",
                                                        lexed_lines[tokenized_line.front()->file].at(
                                                                tokenized_line.front()->at_line - 1
                                                        )
                                                )
                                        );

                                        diag.provide_context(
                                                std::make_unique<Diagnostics::SelectionContext>(
                                                        Diagnostics::FileContext(
                                                                Diagnostics::Variant<std::string>(
                                                                        sym.file,
                                                                        Console::Colour(Console::FOREGROUND_COLOUR::YELLOW)
                                                                ),
                                                                Diagnostics::Variant<size_t>(
                                                                        sym.line_number,
                                                                        Console::Colour(Console::FOREGROUND_COLOUR::YELLOW)
                                                                ),
                                                                Diagnostics::Variant<size_t>(
                                                                        sym.column,
                                                                        Console::Colour(Console::FOREGROUND_COLOUR::YELLOW)
                                                                )
                                                        ), '^', "Previous definition found here",
                                                        lexed_lines[sym.file].at(sym.line_number - 1)
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

                                internal_program_counter += static_cast<uint16_t>(memory_required);
                        }
                        break;
                case Token::DIR_END:
                        end_seen = true;
                        break;
                default:
                        memory_required = memory_requirement_of(tokenized_line.front(), tokenized_line);
                        if (memory_required > -1) {
                                internal_program_counter += static_cast<uint16_t>(memory_required);
                        }
                        break;
                }
        }
}

/*! Assemble the program
 *
 * For each line we have, generate the assembled version of that line. Each line stores its own assembled
 * code, which can be accessed by getting it's assembled vector.
 */
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

/*! Parse the source code given in file.
 *
 * @return 0 on success, anything else for failure.
 */
int Parser::parse()
{
        Lexer lexer(file);
        lexer.lex(tokens, symbols);

        if (!Config::is_set(Config::KEEP_GOING) && Diagnostics::critical()) {
                // There were errors, so don't bother continuing.
                return 1;
        }

        do_first_pass();

        if (!Config::is_set(Config::KEEP_GOING) && Diagnostics::critical()) {
                // There were errors, so don't bother continuing.
                return 1;
        }

        origin_seen = false;
        end_seen    = false;

        program_counter          = internal_program_counter;
        internal_program_counter = 0;

        do_second_pass();

        // Tell the caller whether or not there were errors on the second pass, or not (in which case, the caller
        // can continue the assembly).
        return static_cast<int>(Diagnostics::critical());
}
