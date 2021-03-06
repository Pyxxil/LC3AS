#ifndef PROJECT_LEXER_HPP
#define PROJECT_LEXER_HPP

#include <fstream>
#include <memory>

#include "Diagnostics.hpp"
#include "Extra.hpp"
#include "Line.hpp"
#include "Tokens/Token.hpp"

class Lexer {
public:
  explicit Lexer(const std::string &t_file);
  Lexer(Lexer *t_parent, const std::string &t_file, size_t t_line, size_t t_col,
        size_t len);
  explicit Lexer(Lexer *const other)
      : line(other->line), column(other->column), length(other->length),
        file_name(other->file_name), file(std::move(other->file)),
        symbols(other->symbols), parent(other), tokens(other->tokens) {}
  Lexer(const Lexer &other) = default;
  Lexer(Lexer &&other) = default;

  Lexer &operator=(const Lexer &rhs) = default;
  Lexer &operator=(Lexer &&rhs) = default;

  ~Lexer();

  void lex(std::vector<std::vector<std::shared_ptr<Token>>> &t_tokens,
           std::map<std::string, Symbol> &t_symbols);

  static std::shared_ptr<Token> tokenize(std::string word,
                                         const std::string &file_name,
                                         size_t line_number, size_t t_column);

  static std::shared_ptr<Token>
  tokenize_immediate_or_label(std::string word, std::string copy,
                              const std::string &file_name, size_t line_number,
                              size_t t_column);

  static std::vector<std::shared_ptr<Token>>
  tokenize_line(Line current_line, const std::string &file_name,
                size_t line_number);

  /*! Add a token to the current line's tokens.
   *
   * @param token The string containing the token.
   * @param to The current tokens in the line.
   * @param line_number The line number (only relevant for working with
   * files).
   * @param col The column in the file that the token was found at
   */
  static void add_token(std::string &token,
                        std::vector<std::shared_ptr<Token>> &to,
                        const std::string &file_name, size_t line_number,
                        size_t col) {
    if (!token.empty()) {
      // This effectively erases the token too, which saves a call to
      // token.erase()
      to.push_back(tokenize(std::move(token), file_name, line_number, col));
    }
  }

  inline const auto &get_tokens() const { return tokens; }

  inline const auto &get_symbols() const { return symbols; }

  void provide_context(Diagnostics::Diagnostic &diagnostic);

private:
  size_t line;
  size_t column;
  size_t length;

  std::string file_name;
  std::ifstream file;
  std::map<std::string, Symbol> symbols;
  Lexer *const parent;

  bool is_fail = false;

  std::vector<std::vector<std::shared_ptr<Token>>> tokens;
  void include_file(std::vector<std::shared_ptr<Token>> &t_line);
};

#endif
