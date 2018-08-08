#include "Parser.hpp"

#include "Configuration.hpp"
#include "Diagnostics.hpp"
#include "LexHelper.hpp"
#include "Lexer.hpp"
#include "Tokens/Tokens.hpp"

/**
 * Make our best guess at how much memory a line requires.
 *
 * @param t_token The token currently being processed.
 * @param t_tokens All of the tokens on that line (needed for calling
 * guess_memory_size on token)
 *
 * @returns The best guess we have for how much memory the line requires.
 */
uint16_t
Parser::memory_requirement_of(const std::shared_ptr<Token> &t_token,
                              std::vector<std::shared_ptr<Token>> &t_tokens) {
  if (!origin_seen) {
    t_token->expected(".ORIG statement");
    return 0u;
  }

  if (end_seen) {
    Diagnostics::Diagnostic diagnostic(
        Diagnostics::FileContext(t_token->file, t_token->line, t_token->column),
        "Token after .END directive, it will be ignored.", Diagnostics::SYNTAX,
        Diagnostics::WARNING);

    diagnostic.provide_context(std::make_unique<Diagnostics::HighlightContext>(
        Diagnostics::SelectionContext(
            Diagnostics::FileContext(end->file, end->line, end->column), '^',
            ".END directive defined here", lexed_lines[end->file][end->line]),
        '~', end->token.length()));

    Diagnostics::push(diagnostic);
    return 0u;
  }

  if (t_token->valid_arguments(t_tokens)) {
    return t_token->guess_memory_size(t_tokens);
  }

  return 0u;
}

/*! Generate the symbol table
 *
 * Create the symbol table, given a starting address by a .ORIG directive. If
 * none is found, this currently treats that as an error (the same happens if an
 * instruction of some kind is found before a .ORIG statement).
 *
 * The position in memory for each instruction is dependent on our best guess as
 * to how much memory each instruction between each label.
 */
void Parser::do_first_pass() {
  // TODO: As it is, if there is no .ORIG directive as the first instruction
  // TODO: in the file, we still look at the rest of the file. Is it smarter to
  // TODO: stop if we don't see it first? Or should we just default to setting
  // TODO: the memory address to 0x3000?
  for (auto &&tokenized_line : tokens) {
    switch (auto &&token = tokenized_line.front(); token->type()) {
    case Token::DIR_ORIG: {
      if (origin_seen) {
        Diagnostics::Diagnostic diagnostic(
            Diagnostics::FileContext(token->file, token->line, token->column),
            "Redefinition of Origin memory address",
            Diagnostics::MULTIPLE_DEFINITIONS, Diagnostics::WARNING);

        diagnostic.provide_context(
            std::make_unique<Diagnostics::HighlightContext>(
                Diagnostics::SelectionContext(
                    Diagnostics::FileContext(origin->file, origin->line,
                                             origin->column),
                    '^', "Previous definition found here",
                    lexed_lines[origin->file][origin->line]),
                '~', origin->token.length()));

        Diagnostics::push(diagnostic);
        break;
      }
      origin_seen = true;
      origin = token;
      internal_program_counter += memory_requirement_of(token, tokenized_line);
      break;
    }
    case Token::LABEL: {
      const uint16_t memory_req = memory_requirement_of(token, tokenized_line);
      std::static_pointer_cast<Label>(token)->address =
          internal_program_counter;

      for (auto &&[_, symbol] : symbols) {
        if (symbol.address == internal_program_counter) {
          Diagnostics::Diagnostic diagnostic(
              Diagnostics::FileContext(token->file, token->line, token->column),
              "Multiple labels found for address",
              Diagnostics::MULTIPLE_DEFINITIONS, Diagnostics::ERROR);

          diagnostic.provide_context(
              std::make_unique<Diagnostics::SelectionContext>(
                  Diagnostics::FileContext(symbol.file, symbol.line_number,
                                           symbol.column),
                  '^', "Previous label found here",
                  lexed_lines[symbol.file][symbol.line_number]));

          Diagnostics::push(diagnostic);
          break;
        }
      }

      auto &&[symbol, inserted] = symbols.try_emplace(
          token->token, Symbol(internal_program_counter, token->line,
                               token->column, token->file));
      if (!inserted) {
        // TODO: Fix the way these are handled. At the moment, any errors
        // TODO: thrown here from labels that have been included (from the
        // TODO: same file) won't actually be useful due to the fact that it
        // TODO: doesn't tell the user where the .include was found.
        auto &&sym = symbol->second;
        Diagnostics::Diagnostic diagnostic(
            Diagnostics::FileContext(token->file, token->line, token->column),
            "Multiple definitions of label", Diagnostics::MULTIPLE_DEFINITIONS,
            Diagnostics::ERROR);

        diagnostic.provide_context(
            std::make_unique<Diagnostics::SelectionContext>(
                Diagnostics::FileContext(token->file, token->line,
                                         token->column),
                '^', "Redefinition of this label",
                lexed_lines[token->file][token->line]));

        diagnostic.provide_context(
            std::make_unique<Diagnostics::SelectionContext>(
                Diagnostics::FileContext(sym.file, sym.line_number, sym.column),
                '^', "Previous definition found here",
                lexed_lines[sym.file][sym.line_number]));

        Diagnostics::push(diagnostic);
      } else {
        longest_symbol_length = std::max(
            longest_symbol_length, static_cast<int>(token->token.length()));
      }

      internal_program_counter += memory_req;
      break;
    }
    case Token::DIR_END: {
      if (end_seen) {
        Diagnostics::Diagnostic diagnostic(
            Diagnostics::FileContext(token->file, token->line, token->column),
            "Extra .END directive", Diagnostics::MULTIPLE_DEFINITIONS,
            Diagnostics::WARNING);

        diagnostic.provide_context(
            std::make_unique<Diagnostics::HighlightContext>(
                Diagnostics::SelectionContext(
                    Diagnostics::FileContext(end->file, end->line, end->column),
                    '^', "Previous definition found here",
                    lexed_lines[end->file][end->line]),
                '~', end->token.length()));

        Diagnostics::push(diagnostic);
      } else {
        end = token;
        end_seen = true;
      }
      break;
    }
    default:
      internal_program_counter += memory_requirement_of(token, tokenized_line);
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
void Parser::do_second_pass() {
  for (auto &&tokenized_line : tokens) {
    auto &&token = tokenized_line.front();
    if (token->type() == Token::DIR_END) {
      end_seen = true;
      break;
    }

    internal_program_counter += static_cast<uint16_t>(
        token->assemble(tokenized_line, symbols, internal_program_counter));
  }
}

/*! Parse the source code given in file.
 *
 * @return 0 on success, anything else for failure.
 */
bool Parser::parse() {
  Lexer lexer(file);
  lexer.lex(tokens, symbols);

  if (!Config::is_set(Config::KEEP_GOING) && Diagnostics::critical()) {
    // There were errors, so don't bother continuing.
    return false;
  }

  do_first_pass();

  if (!Config::is_set(Config::KEEP_GOING) && Diagnostics::critical()) {
    // There were errors, so don't bother continuing.
    return false;
  }

  origin_seen = false;
  end_seen = false;

  program_counter = internal_program_counter;
  internal_program_counter = 0;

  do_second_pass();

  // Tell the caller whether there were errors on the second pass, or not (in
  // which case, the caller can continue the assembly).
  return Diagnostics::critical();
}
