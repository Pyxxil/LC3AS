#include "Tokens/Instructions/Token_Instruction_Jsrr.hpp"

#include <fmt/ostream.h>

#include "Tokens/Token_Register.hpp"

Jsrr::Jsrr(const std::string& instruction,
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
Jsrr::assemble(std::vector<std::shared_ptr<Token>>& tokens,
               const std::map<std::string, Symbol>& symbols,
               uint16_t program_counter)
{
  (void)symbols;
  (void)program_counter;

  if (!is_valid) {
    return -1;
  }

  assembled.emplace_back(static_cast<uint16_t>(
    0x4000 | (std::static_pointer_cast<Register>(tokens[1])->reg << 5)));

  return 1;
}

bool
Jsrr::valid_arguments(std::vector<std::shared_ptr<Token>>& tokens)
{
  if (tokens.size() != 2) {
    invalid_argument_count(
      tokens.size(), 1, tokens.back()->column + tokens.back()->token.length());
    return (is_valid = false);
  }

  if (tokens[1]->type() != Token::LABEL) {
    tokens[1]->expected("label");
    return (is_valid = false);
  }

  return is_valid;
}

uint16_t
Jsrr::guess_memory_size(std::vector<std::shared_ptr<Token>>& tokens) const
{
  (void)tokens;
  return static_cast<uint16_t>(is_valid);
}

std::string
Jsrr::disassemble(uint16_t& program_counter,
                  const std::string& symbol,
                  int width) const
{
  const auto value = assembled.front();

#ifdef INCLUDE_ADDONS
  return fmt::format(
    "({0:04X}) {1:04X} {1:016b} ({2: >4d}) {3: <{4}s} JSRR R{5:d}\t{6:s}\n",
    program_counter++,
    value,
    line,
    symbol,
    width,
    value >> 6 & 0x7,
    file);
#else
  return fmt::format(
    "({0:04X}) {1:04X} {1:016b} ({2: >4d}) {3: <{4}s} JSRR R{5:d}\n",
    program_counter++,
    value,
    line,
    symbol,
    width,
    value >> 6 & 0x7);
#endif
}
