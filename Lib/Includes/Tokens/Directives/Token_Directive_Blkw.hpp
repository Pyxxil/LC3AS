#ifndef TOKEN_DIRECTIVE_BLKW_HPP
#define TOKEN_DIRECTIVE_BLKW_HPP

#include "Tokens/Token_Directive.hpp"
#include "Tokens/Token_Immediate.hpp"

class Blkw : public Directive
{
public:
  Blkw(std::string& directive,
       std::string& directive_uppercase,
       std::string& t_file,
       size_t line_number,
       size_t column);

  Token::token_type type() const override;

  int32_t assemble(std::vector<std::shared_ptr<Token>>& tokens,
                   const std::map<std::string, Symbol>& symbols,
                   uint16_t program_counter) override;
  uint16_t guess_memory_size(
    std::vector<std::shared_ptr<Token>>& tokens) const override;

  void invalid_argument_count(size_t provided,
                              size_t expected,
                              size_t last_column) const override;

  bool valid_arguments(std::vector<std::shared_ptr<Token>>& tokens) override;

  std::string disassemble(uint16_t& program_counter,
                          const std::string& symbol,
                          int width) const override;
};

#endif // TOKEN_DIRECTIVE_BLKW_HPP
