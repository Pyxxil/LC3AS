#include "Tokens/Instructions/Token_Instruction_Ret.hpp"

#include <fmt/ostream.h>

Ret::Ret(const std::string& instruction,
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
Ret::assemble(std::vector<std::shared_ptr<Token>>& tokens,
              const std::map<std::string, Symbol>& symbols,
              uint16_t program_counter)
{
  (void)tokens;
  (void)symbols;
  (void)program_counter;

  assembled.emplace_back(static_cast<uint16_t>(0xC1C0));

  return 1;
}

bool
Ret::valid_arguments(std::vector<std::shared_ptr<Token>>& tokens)
{
  if (tokens.size() != 1) {
    invalid_argument_count(
      tokens.size(), 0, tokens.back()->column + tokens.back()->token.length());
    return (is_valid = false);
  }

  return is_valid;
}

uint16_t
Ret::guess_memory_size(std::vector<std::shared_ptr<Token>>& tokens) const
{
  (void)tokens;
  return static_cast<uint16_t>(is_valid);
}

std::string
Ret::disassemble(uint16_t& program_counter,
                 const std::string& symbol,
                 int width) const
{
  const auto value = assembled.front();

#ifdef INCLUDE_ADDONS
  return fmt::format(
    "({0:04X}) {1:04X} {1:016b} ({2: >4d}) {3: <{4}s} RET\t{5:s}\n",
    program_counter++,
    value,
    line,
    symbol,
    width,
    file);
#else
  return fmt::format("({0:04X}) {1:04X} {1:016b} ({2: >4d}) {3: <{4}s} RET\n",
                     program_counter++,
                     value,
                     line,
                     symbol,
                     width);
#endif
}
