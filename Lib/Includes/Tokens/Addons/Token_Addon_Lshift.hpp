#ifndef TOKEN_ADDON_LSHIFT_HPP
#define TOKEN_ADDON_LSHIFT_HPP

#include "Tokens/Instructions/Token_Instruction_Add.hpp"
#include "Tokens/Token_Directive.hpp"

/**
 * Lshift looks like so:
 * .LSHIFT <Register>,? <uimm4>
 * The register can be any of the 8 normal registers, however imm4 can only be
 * in the range 0 - 15 (inclusive). THis is due to the fact that each register
 * is only 16 bits wide, and so any extra shifting would be pointless.
 *
 * TODO: Do we let the value be greater than 15, in which case it just evaluates
 * to setting it to 0?
 */

class Lshift : public Directive
{
public:
  Lshift(std::string& directive,
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
                          int width) const override final;

private:
  std::shared_ptr<Add> add;
};

#endif // TOKEN_ADDON_LSHIFT_HPP
