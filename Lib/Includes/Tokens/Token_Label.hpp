#ifndef TOKEN_LABEL_HPP
#define TOKEN_LABEL_HPP

#include "Tokens/Token.hpp"

class Label : public Token
{
public:
  Label(const std::string& label,
        const std::string& t_file,
        size_t line_number,
        size_t t_column);

  Token::token_type type() const final;

  int32_t assemble(std::vector<std::shared_ptr<Token>>& tokens,
                   const std::map<std::string, Symbol>& symbols,
                   uint16_t program_counter) final;
  uint16_t guess_memory_size(
    std::vector<std::shared_ptr<Token>>& tokens) const final;

  bool valid_arguments(std::vector<std::shared_ptr<Token>>& tokens) final;

  void not_found(const std::map<std::string, Symbol>& match_candidates);
  void requires_too_many_bits(
    int allowed_bits,
    bool is_signed,
    const Token* caller,
    const std::map<std::string, Symbol>& symbols) final;

  std::string disassemble(uint16_t& program_counter,
                          const std::string& symbol,
                          int width) const final;

  uint16_t address = 0;

private:
  std::shared_ptr<Token> instruction;
};

#endif // TOKEN_LABEL_HPP
