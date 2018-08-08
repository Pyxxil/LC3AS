#include "Tokens/Instructions/Token_Instruction_Trap.hpp"

#include <fmt/ostream.h>

#include "Tokens/Token_Immediate.hpp"

Trap::Trap(const std::string& instruction,
           const std::string& instruction_uppercase,
           const std::string& t_file,
           size_t line_number,
           size_t t_column)
  : Instruction(instruction,
                instruction_uppercase,
                t_file,
                line_number,
                t_column)
{}

int32_t
Trap::assemble(std::vector<std::shared_ptr<Token>>& tokens,
               const std::map<std::string, Symbol>& symbols,
               uint16_t program_counter)
{
  (void)symbols;
  (void)program_counter;

  if (!is_valid) {
    return -1;
  }

  assembled.emplace_back(static_cast<uint16_t>(
    0xF000 | (std::static_pointer_cast<Immediate>(tokens[1])->value & 0xFF)));

  return 1;
}

bool
Trap::valid_arguments(std::vector<std::shared_ptr<Token>>& tokens)
{
  if (tokens.size() != 2) {
    invalid_argument_count(
      tokens.size(), 1, tokens.back()->column + tokens.back()->token.length());
    return (is_valid = false);
  }

  if (tokens[1]->type() != Token::IMMEDIATE) {
    tokens[1]->expected("valid trap vector");
    return (is_valid = false);
  }

  if (!tokens[1]->is_valid) {
    return (is_valid = false);
  }

  if (std::static_pointer_cast<Immediate>(tokens[1])->value > 0xFF) {
    tokens[1]->requires_too_many_bits(
      8, UNSIGNED, this, std::map<std::string, Symbol>());
    return (is_valid = false);
  }

  return is_valid;
}

uint16_t
Trap::guess_memory_size(std::vector<std::shared_ptr<Token>>& tokens) const
{
  (void)tokens;
  return static_cast<uint16_t>(is_valid);
}

std::string
Trap::disassemble(uint16_t& program_counter,
                  const std::string& symbol,
                  int width) const
{
  const auto value = static_cast<int>(assembled.front());

#ifdef INCLUDE_ADDONS
  return fmt::format("({0:04X}) {1:04X} {1:016b} ({2: >4d}) {3: <{4}s} TRAP "
                     "{1:#06X}\t{5:s}\n",
                     program_counter++,
                     value,
                     line,
                     symbol,
                     width,
                     file);
#else
  return fmt::format(
    "({0:04X}) {1:04X} {1:016b} ({3: >4d}) {4: <{5}s} TRAP {1:#06X}\n",
    program_counter++,
    value,
    line,
    symbol,
    width);
#endif
}
