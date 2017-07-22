#ifndef TOKEN_ADDON_NEG_HPP
#define TOKEN_ADDON_NEG_HPP

#include "Tokens/Instructions/Token_Instruction_Add.hpp"
#include "Tokens/Instructions/Token_Instruction_Not.hpp"
#include "Tokens/Token_Directive.hpp"

class Neg : public Directive
{
public:
  Neg(std::string& directive,
      std::string& directive_uppercase,
      std::string& t_file,
      size_t line_number,
      size_t column);
  Neg(std::string&& directive,
      std::string&& directive_uppercase,
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
  std::shared_ptr<Not> neg;
  std::shared_ptr<Add> add;
};

#endif // TOKEN_ADDON_NEG_HPP
