#include "Tokens/Instructions/Token_Instruction_Jsr.hpp"

#include <fmt/ostream.h>

#include "Tokens/Token_Immediate.hpp"
#include "Tokens/Token_Label.hpp"

Jsr::Jsr(const std::string& instruction,
         const std::string& instruction_uppercase,
         const std::string& t_file,
         size_t line_number,
         size_t t_column)
  : Instruction(instruction,
                instruction_uppercase,
                t_file,
                line_number,
                t_column)
  , provided()
{}

int32_t
Jsr::assemble(std::vector<std::shared_ptr<Token>>& tokens,
              const std::map<std::string, Symbol>& symbols,
              uint16_t program_counter)
{
  if (!is_valid) {
    return -1;
  }

  int offset = 0;

  if (tokens[1]->type() == Token::LABEL) {
    if (0u == symbols.count(tokens[1]->token)) {
      std::static_pointer_cast<Label>(tokens[1])->not_found(symbols);
      return -1;
    }

    offset = static_cast<int>(symbols.find(tokens[1]->token)->second.address) -
             (static_cast<int>(program_counter) + 1);
  } else {
    offset = std::static_pointer_cast<Immediate>(tokens[1])->value;
  }

  if (offset > 1023 || offset < -1024) {
    tokens[1]->requires_too_many_bits(11, SIGNED, this, symbols);
    return -1;
  }

  provided = tokens[1];
  assembled.emplace_back(0x4800 | static_cast<uint16_t>(offset & 0x7FF));

  return 1;
}

bool
Jsr::valid_arguments(std::vector<std::shared_ptr<Token>>& tokens)
{
  if (tokens.size() != 2) {
    invalid_argument_count(
      tokens.size(), 1, tokens.back()->column + tokens.back()->token.length());
    return (is_valid = false);
  }

  if (tokens[1]->type() != Token::LABEL &&
      tokens[1]->type() != Token::IMMEDIATE) {
    tokens[1]->expected("label or value");
    return (is_valid = false);
  }

  return is_valid;
}

uint16_t
Jsr::guess_memory_size(std::vector<std::shared_ptr<Token>>& tokens) const
{
  (void)tokens;
  return static_cast<uint16_t>(is_valid);
}

std::string
Jsr::disassemble(uint16_t& program_counter,
                 const std::string& symbol,
                 int width) const
{
  const auto value = assembled.front();

  ++program_counter;

#ifdef INCLUDE_ADDONS
  return fmt::format(
    "({0:04X}) {1:04X} {1:016b} ({2: >4d}) {3: <{4}s} JSR {5:s}\t{6:s}\n",
    program_counter - 1,
    value,
    line,
    symbol,
    width,
    provided->type() == Token::LABEL
      ? provided->token
      : fmt::format("{:#06x}",
                    std::static_pointer_cast<Immediate>(provided)->value +
                      program_counter),
    file);
#else
  return fmt::format(
    "({0:04X}) {1:04X} {1:016b} ({2: >4d}) {3: <{4}s} JSR {5:s}\n",
    program_counter - 1,
    value,
    line,
    symbol,
    width,
    provided->type() == Token::LABEL
      ? provided->token
      : fmt::format("{:#06x}",
                    std::static_pointer_cast<Immediate>(provided)->value +
                      program_counter));
#endif
}
