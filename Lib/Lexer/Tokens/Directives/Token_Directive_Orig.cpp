#include "Tokens/Directives/Token_Directive_Orig.hpp"

#include <fmt/ostream.h>

Orig::Orig(const std::string& directive,
           const std::string& directive_uppercase,
           const std::string& t_file,
           size_t line_number,
           size_t t_column)
  : Directive(directive, directive_uppercase, t_file, line_number, t_column)
{}

int32_t
Orig::assemble(std::vector<std::shared_ptr<Token>>& tokens,
               const std::map<std::string, Symbol>& symbols,
               uint16_t program_counter)
{
  (void)symbols;
  (void)program_counter;

  if (!is_valid) {
    return -1;
  }

  origin = static_cast<uint16_t>(
    std::static_pointer_cast<Immediate>(tokens[1])->value);
  assembled.emplace_back(origin);

  return origin;
}

bool
Orig::valid_arguments(std::vector<std::shared_ptr<Token>>& tokens)
{
  if (tokens.size() != 2) {
    invalid_argument_count(
      tokens.size(), 1, tokens.back()->column + tokens.back()->token.length());
    return (is_valid = false);
  }

  if (tokens[1]->type() != Token::IMMEDIATE) {
    tokens[1]->expected("immediate value");
    return (is_valid = false);
  }

  if (!tokens[1]->is_valid) {
    return (is_valid = false);
  }

  return is_valid;
}

uint16_t
Orig::guess_memory_size(std::vector<std::shared_ptr<Token>>& tokens) const
{
  if (is_valid) {
    return static_cast<uint16_t>(static_cast<uint16_t>(
      std::static_pointer_cast<Immediate>(tokens[1])->value));
  }

  return static_cast<uint16_t>(0);
}

std::string
Orig::disassemble(uint16_t& program_counter,
                  const std::string& symbol,
                  int width) const
{
  // You can't have a symbol here...
  (void)symbol;

  program_counter = assembled.front();

#ifdef INCLUDE_ADDONS
  return fmt::format("(0000) {0:04x} {0:016b} ({1: >4d}) {2: <{3}s} "
                     ".ORIG {0:#04x}\t{4:s}\n",
                     program_counter,
                     line,
                     std::string(),
                     width,
                     file);
#else
  return fmt::format("(0000) {0:04x} {0:016b} ({1: >4d}) {2: <{3}s} "
                     ".ORIG {0:#06x}\n",
                     program_counter,
                     line,
                     std::string(),
                     width);
#endif
}
