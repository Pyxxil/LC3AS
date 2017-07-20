#include "Parser.hpp"

#include "Configuration.hpp"
#include "Diagnostics.hpp"
#include "LexHelper.hpp"
#include "Lexer.hpp"
#include "Tokens/Tokens.hpp"

/*! Generate the symbol table
 *
 * Create the symbol table, given a starting address by a .ORIG directive. If
 * none is found, this currently treats that as an error (the same happens if an
 * instruction of some kind is found before a .ORIG statement).
 *
 * The position in memory for each instruction is dependent on our best guess as
 * to how much memory each instruction between each label.
 */
void
Parser::do_first_pass()
{
  /**
   * Make our best guess at how much memory a line requires.
   *
   * @param token: The token currently being processed.
   * @param t_tokens: All of the tokens on that line (needed for calling
   * guess_memory_size on token)
   *
   * @returns The best guess we have for how much memory the line requires.
   */
  const auto& memory_requirement_of = [this](const auto& token,
                                             auto&& t_tokens) -> size_t {
    if (!origin_seen) {
      token->expected(".ORIG statement");
      return 0;
    }

    if (end_seen) {
      // TODO: Do we show the user where the .END directive was?
      Diagnostics::Diagnostic diagnostic(
        Diagnostics::FileContext(token->file, token->line, token->column),
        "Token after .END directive, it will be ignored.",
        Diagnostics::SYNTAX,
        Diagnostics::WARNING);

      diagnostic.provide_context(
        std::make_unique<Diagnostics::HighlightContext>(
          Diagnostics::SelectionContext(
            Diagnostics::FileContext(end->file, end->line, end->column),
            '^',
            ".END directive defined here",
            lexed_lines[end->file].at(end->line)),
          '~',
          end->token.length()));

      Diagnostics::push(diagnostic);
      return 0;
    }

    if (token->valid_arguments(t_tokens)) {
      return token->guess_memory_size(t_tokens);
    }

    return 0;
  };

  // TODO: As it is, if there is no .ORIG directive as the first instruction
  // in the file,
  // TODO: we still look at the rest of the file. Is it smarter to stop if we
  // don't see it
  // TODO: first? Or should we just default to setting the memory address to
  // 0x3000?
  for (auto&& tokenized_line : tokens) {
    switch (tokenized_line.front()->type()) {
      case Token::DIR_ORIG: {
        if (origin_seen) {
          Diagnostics::Diagnostic diagnostic(
            Diagnostics::FileContext(tokenized_line.front()->file,
                                     tokenized_line.front()->line,
                                     tokenized_line.front()->column),
            "Redefinition of Origin memory address",
            Diagnostics::MULTIPLE_DEFINITIONS,
            Diagnostics::WARNING);

          diagnostic.provide_context(
            std::make_unique<Diagnostics::HighlightContext>(
              Diagnostics::SelectionContext(
                Diagnostics::FileContext(
                  origin->file, origin->line, origin->column),
                '^',
                "Previous definition found here",
                lexed_lines[origin->file].at(origin->line)),
              '~',
              origin->token.length()));

          Diagnostics::push(diagnostic);
          break;
        }
        origin_seen = true;
        origin = tokenized_line.front();
        internal_program_counter +=
          memory_requirement_of(tokenized_line.front(), tokenized_line);
        break;
      }
      case Token::LABEL: {
        auto memory_req =
          memory_requirement_of(tokenized_line.front(), tokenized_line);
        std::static_pointer_cast<Label>(tokenized_line.front())->address =
          internal_program_counter;

        for (const auto& symbol : symbols) {
          if (symbol.second.address == internal_program_counter) {
            Diagnostics::Diagnostic diagnostic(
              Diagnostics::FileContext(tokenized_line.front()->file,
                                       tokenized_line.front()->line,
                                       tokenized_line.front()->column),
              "Multiple labels found for address",
              Diagnostics::MULTIPLE_DEFINITIONS,
              Diagnostics::ERROR);

            diagnostic.provide_context(
              std::make_unique<Diagnostics::SelectionContext>(
                Diagnostics::FileContext(symbol.second.file,
                                         symbol.second.line_number,
                                         symbol.second.column),
                '^',
                "Previous label found here",
                lexed_lines[symbol.second.file].at(symbol.second.line_number)));

            Diagnostics::push(diagnostic);
            break;
          }
        }

        if (0u != symbols.count(tokenized_line.front()->token)) {
          auto&& sym = symbols.at(tokenized_line.front()->token);
          Diagnostics::Diagnostic diagnostic(
            Diagnostics::FileContext(tokenized_line.front()->file,
                                     tokenized_line.front()->line,
                                     tokenized_line.front()->column),
            "Multiple definitions of label",
            Diagnostics::MULTIPLE_DEFINITIONS,
            Diagnostics::ERROR);

          diagnostic.provide_context(
            std::make_unique<Diagnostics::SelectionContext>(
              Diagnostics::FileContext(tokenized_line.front()->file,
                                       tokenized_line.front()->line,
                                       tokenized_line.front()->column),
              '^',
              "Redefinition of this label",
              lexed_lines[tokenized_line.front()->file].at(
                tokenized_line.front()->line)));

          diagnostic.provide_context(
            std::make_unique<Diagnostics::SelectionContext>(
              Diagnostics::FileContext(sym.file, sym.line_number, sym.column),
              '^',
              "Previous definition found here",
              lexed_lines[sym.file].at(sym.line_number)));

          Diagnostics::push(diagnostic);
        }

        symbols.insert(
          std::pair<std::string, Symbol>(tokenized_line.front()->token,
                                         Symbol(internal_program_counter,
                                                tokenized_line.front()->line,
                                                tokenized_line.front()->column,
                                                tokenized_line.front()->file)));

        longest_symbol_length =
          std::max(longest_symbol_length,
                   static_cast<int>(tokenized_line.front()->token.length()));

        internal_program_counter += memory_req;
        break;
      }
      case Token::DIR_END: {
        if (end_seen) {
          Diagnostics::Diagnostic diagnostic(
            Diagnostics::FileContext(tokenized_line.front()->file,
                                     tokenized_line.front()->line,
                                     tokenized_line.front()->column),
            "Extra .END directive",
            Diagnostics::MULTIPLE_DEFINITIONS,
            Diagnostics::WARNING);

          diagnostic.provide_context(
            std::make_unique<Diagnostics::HighlightContext>(
              Diagnostics::SelectionContext(
                Diagnostics::FileContext(end->file, end->line, end->column),
                '^',
                "Previous definition found here",
                lexed_lines[end->file].at(end->line)),
              '~',
              end->token.length()));

          Diagnostics::push(diagnostic);
        } else {
          end = tokenized_line.front();
          end_seen = true;
        }
        break;
      }
      default:
        internal_program_counter +=
          memory_requirement_of(tokenized_line.front(), tokenized_line);
        break;
    }
  }
}

/*! Perform assembly of the file
 *
 * For each line we have, generate the assembled version of that line. Each line
 * stores its own assembled code, which can be accessed by getting it's
 * assembled vector.
 */
void
Parser::do_second_pass()
{
  int memory_required = 0;

  for (auto&& tokenized_line : tokens) {
    if (tokenized_line.front()->type() == Token::DIR_END) {
      end_seen = true;
      break;
    }

    memory_required = tokenized_line.front()->assemble(
      tokenized_line, symbols, internal_program_counter);

    if (memory_required >= 0) {
      internal_program_counter += static_cast<uint16_t>(memory_required);
    }
  }
}

/*! Parse the source code given in file.
 *
 * @return 0 on success, anything else for failure.
 */
int
Parser::parse()
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
  end_seen = false;

  program_counter = internal_program_counter;
  internal_program_counter = 0;

  do_second_pass();

  // Tell the caller whether there were errors on the second pass, or not (in
  // which case, the caller can continue the assembly).
  return static_cast<int>(Diagnostics::critical());
}
