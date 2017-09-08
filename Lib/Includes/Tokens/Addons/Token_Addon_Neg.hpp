#ifndef TOKEN_ADDON_NEG_HPP
#define TOKEN_ADDON_NEG_HPP

#include "Tokens/Instructions/Token_Instruction_Add.hpp"
#include "Tokens/Instructions/Token_Instruction_Not.hpp"
#include "Tokens/Token_Directive.hpp"

/**
 * Neg looks like this
 * .NEG <register>
 * The register can be any of the 8 normal registers.
 *
 * NEG will convert the value in the register to it's two's compliment.
 *
 * TODO: Implement .NEG <reg>, <reg> which should do the following:
 *      .NEG R1, R2 turns into
 *       ADD  R2, R1, #0
 *       .NEG R2  // Seems weird, but technically works better.
 */

class Neg : public Directive
{
public:
  Neg(const std::string& directive,
      const std::string& directive_uppercase,
      const std::string& t_file,
      size_t line_number,
      size_t column);
  Neg(std::string&& directive,
      std::string&& directive_uppercase,
      const std::string& t_file,
      size_t line_number,
      size_t column);

  token_type type() const override final;

  int32_t assemble(std::vector<std::shared_ptr<Token>>& tokens,
                   const std::map<std::string, Symbol>& symbols,
                   uint16_t program_counter) final;
  uint16_t guess_memory_size(
    std::vector<std::shared_ptr<Token>>& tokens) const final;

  bool valid_arguments(std::vector<std::shared_ptr<Token>>& tokens) final;

  std::string disassemble(uint16_t& program_counter,
                          const std::string& symbol,
                          int width) const final;

private:
  std::shared_ptr<Not> neg;
  std::shared_ptr<Add> add;
};

#endif // TOKEN_ADDON_NEG_HPP
