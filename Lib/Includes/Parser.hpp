#ifndef PROJECT_PARSER_HPP
#define PROJECT_PARSER_HPP

#include <map>
#include <string>
#include <utility>

#include "Tokens/Symbol.hpp"
#include "Tokens/Token.hpp"

class Parser {
public:
  explicit Parser(std::string t_file)
      : file(std::move(t_file)), origin_seen(false), end_seen(false),
        internal_program_counter(0), program_counter(0),
        longest_symbol_length(20), symbols(), tokens(), origin(), end() {}
  Parser(const Parser &other) = default;
  Parser(Parser &&other) noexcept = default;

  Parser &operator=(const Parser &rhs) = default;
  Parser &operator=(Parser &&rhs) noexcept = default;

  ~Parser() = default;

  bool parse();

  uint16_t starting_address() const { return program_counter; }

  inline const auto &parsed_tokens() const { return tokens; }

  inline const auto &parsed_symbols() const { return symbols; }

  inline int symbol_padding() const { return longest_symbol_length; }

private:
  std::string file;

  bool origin_seen;
  bool end_seen;

  uint16_t internal_program_counter;
  uint16_t program_counter;

  int longest_symbol_length;

  std::map<std::string, Symbol> symbols;

  std::vector<std::vector<std::shared_ptr<Token>>> tokens;

  std::shared_ptr<Token> origin;
  std::shared_ptr<Token> end;

  void do_first_pass();
  void do_second_pass();

  uint16_t memory_requirement_of(const std::shared_ptr<Token> &t_token,
                                 std::vector<std::shared_ptr<Token>> &t_tokens);
};

#endif
