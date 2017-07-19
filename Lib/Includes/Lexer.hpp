#ifndef PROJECT_LEXER_HPP
#define PROJECT_LEXER_HPP

#include <fstream>
#include <memory>

#include "Diagnostics.hpp"
#include "Tokens/Token.hpp"

class Lexer
{
public:
  explicit Lexer(const std::string& t_file);
  Lexer(Lexer* t_parent,
        const std::string& t_file,
        size_t line,
        size_t col,
        size_t len);
  explicit Lexer(Lexer* const other)
    : line(other->line)
    , column(other->column)
    , length(other->length)
    , file_name(other->file_name)
    , file(std::move(other->file))
    , symbols(other->symbols)
    , parent(other)
    , tokens(other->tokens)
  {}
  Lexer(const Lexer& other) = default;
  Lexer(Lexer&& other) = default;

  Lexer& operator=(const Lexer& rhs) = default;
  Lexer& operator=(Lexer&& rhs) = default;

  ~Lexer();

  void lex(std::vector<std::vector<std::shared_ptr<Token>>>& t_tokens,
           std::map<std::string, Symbol>& t_symbols);
  std::shared_ptr<Token> tokenize(std::string word,
                                  size_t line_number,
                                  size_t column);
  void tokenizeLine(std::string line,
                    size_t line_number,
                    std::vector<std::shared_ptr<Token>>& into);
  void addToken(std::string& token,
                std::vector<std::shared_ptr<Token>>& to,
                size_t line_number,
                size_t col);

  inline const auto& get_tokens() const { return tokens; }

  inline const auto& get_symbols() const { return symbols; }

  void provide_context(Diagnostics::Diagnostic& diagnostic);

private:
  size_t line;
  size_t column;
  size_t length;

  std::string file_name;
  std::ifstream file;
  std::map<std::string, Symbol> symbols;
  Lexer* const parent;

  std::vector<std::vector<std::shared_ptr<Token>>> tokens;
};

#endif
