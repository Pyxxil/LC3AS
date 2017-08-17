#ifndef TOKEN_ADDON_SUB_HPP
#define TOKEN_ADDON_SUB_HPP

#include "Tokens/Token_Directive.hpp"

#include "Tokens/Addons/Token_Addon_Neg.hpp"
#include "Tokens/Immediates/Token_Immediate_Decimal.hpp"
#include "Tokens/Instructions/Token_Instruction_Add.hpp"
#include "Tokens/Instructions/Token_Instruction_And.hpp"

/**
 * Sub looks like this:
 * .SUB <reg>,? <reg> (,?<reg>)?
 * <reg> is any of the normal 8 registers, of which there can be either 2 or 3.
 *
 * If sub is only provided with 2 registers, it will subtract the second from
 * the first, and save it in the first. If a third register is provided it will
 * subtrsct the third from the second, and store the result in the first
 * register.
 *
 * TODO: Allow SUB to be provided an immediate value.
 */

class Sub : public Directive
{
public:
  Sub(std::string& directive,
      std::string& directive_uppercase,
      std::string& t_file,
      size_t line_number,
      size_t column);

  token_type type() const override final;

  int32_t assemble(std::vector<std::shared_ptr<Token>>& tokens,
                   const std::map<std::string, Symbol>& symbols,
                   uint16_t program_counter) override final;
  uint16_t guess_memory_size(
    std::vector<std::shared_ptr<Token>>& tokens) const override final;

  bool valid_arguments(
    std::vector<std::shared_ptr<Token>>& tokens) override final;

  std::string disassemble(uint16_t& program_counter,
                          const std::string& symbol,
                          int width) const override;
  void invalid_argument_count(size_t provided,
                              size_t expected,
                              size_t last_column) const override final;

private:
  std::shared_ptr<And> set_zero;
  std::shared_ptr<Add> add;
  std::shared_ptr<Decimal> decimal_zero;
  std::shared_ptr<Neg> neg1;
  std::shared_ptr<Neg> neg2;
};

#endif // TOKEN_ADDON_SUB_HPP
