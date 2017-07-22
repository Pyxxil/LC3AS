#ifndef TOKEN_ADDON_SET_HPP
#define TOKEN_ADDON_SET_HPP

#include "Tokens/Token_Directive.hpp"

#include "Tokens/Directives/Token_Directive_Fill.hpp"
#include "Tokens/Immediates/Token_Immediate_Decimal.hpp"
#include "Tokens/Instructions/Token_Instruction_Add.hpp"
#include "Tokens/Instructions/Token_Instruction_And.hpp"
#include "Tokens/Instructions/Token_Instruction_Br.hpp"
#include "Tokens/Instructions/Token_Instruction_Ld.hpp"

class Set : public Directive
{
public:
  Set(std::string& directive,
      std::string& directive_uppercase,
      std::string& t_file,
      size_t line_number,
      size_t column);

  token_type type() const override;

  int32_t assemble(std::vector<std::shared_ptr<Token>>& tokens,
                   const std::map<std::string, Symbol>& symbols,
                   uint16_t program_counter) override;
  uint16_t guess_memory_size(
    std::vector<std::shared_ptr<Token>>& tokens) const override;

  bool valid_arguments(std::vector<std::shared_ptr<Token>>& tokens) override;

  std::string disassemble(uint16_t& program_counter,
                          const std::string& symbol,
                          int width) const override;

private:
  std::shared_ptr<And> _and;
  std::shared_ptr<Add> add;
  std::shared_ptr<Br> br;
  std::shared_ptr<Ld> ld;
  std::shared_ptr<Fill> fill;
  std::shared_ptr<Token> decimal_zero;
  std::shared_ptr<Decimal> decimal_one;
  std::shared_ptr<Decimal> decimal_negative_two;
};

#endif // TOKEN_ADDON_SET_HPP
