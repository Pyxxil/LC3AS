#include "Tokens/Instructions/Token_Instruction_Ldr.hpp"

#include <fmt/ostream.h>

#include "Tokens/Token_Immediate.hpp"
#include "Tokens/Token_Register.hpp"

Ldr::Ldr(const std::string& instruction,
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
Ldr::assemble(std::vector<std::shared_ptr<Token>>& tokens,
              const std::map<std::string, Symbol>& symbols,
              uint16_t program_counter)
{
  (void)symbols;
  (void)program_counter;

  if (!is_valid) {
    return -1;
  }

  assembled.emplace_back(static_cast<uint16_t>(
    0x6000 | ((std::static_pointer_cast<Register>(tokens[1])->reg & 0x7) << 9) |
    ((std::static_pointer_cast<Register>(tokens[2])->reg & 0x7) << 6) |
    (std::static_pointer_cast<Immediate>(tokens[3])->value & 0x3F)));

  return 1;
}

bool
Ldr::valid_arguments(std::vector<std::shared_ptr<Token>>& tokens)
{
  if (tokens.size() != 4) {
    invalid_argument_count(
      tokens.size(), 3, tokens.back()->column + tokens.back()->token.length());
    return (is_valid = false);
  }

  if (tokens[1]->type() != Token::REGISTER) {
    tokens[1]->expected("register");
    return (is_valid = false);
  }

  if (tokens[2]->type() != Token::REGISTER) {
    tokens[2]->expected("register");
    return (is_valid = false);
  }

  if (tokens[3]->type() != Token::IMMEDIATE) {
    tokens[3]->expected("immediate value");
    return (is_valid = false);
  }

  if (!(tokens[1]->is_valid && tokens[2]->is_valid && tokens[3]->is_valid)) {
    return (is_valid = false);
  }

  if (std::static_pointer_cast<Immediate>(tokens[3])->value > 31 ||
      std::static_pointer_cast<Immediate>(tokens[3])->value < -32) {
    tokens[3]->requires_too_many_bits(
      6, SIGNED, this, std::map<std::string, Symbol>());
    return (is_valid = false);
  }

  return is_valid;
}

uint16_t
Ldr::guess_memory_size(std::vector<std::shared_ptr<Token>>& tokens) const
{
  (void)tokens;
  return static_cast<uint16_t>(is_valid);
}

std::string
Ldr::disassemble(uint16_t& program_counter,
                 const std::string& symbol,
                 int width) const
{
  const auto value = assembled.front();

#ifdef INCLUDE_ADDONS
  return fmt::format(
    "({0:04X}) {1:04X} {1:016b} ({2: >4d}) {3: <{4}s} LDR R{5:d} R{6:d} "
    "#{7:d}\t{8:s}\n",
    program_counter++,
    value,
    line,
    symbol,
    width,
    value >> 9 & 0x7,
    value >> 6 & 0x7,
    (static_cast<int8_t>(value & 0x3F) << 2) >> 2,
    file);
#else
  return fmt::format(
    "({0:04X}) {1:04X} {1:016b} ({2: >4d}) {3: <{4}s} LDR R{5:d} R{6:d} "
    "#{7:d}\n",
    program_counter++,
    value,
    line,
    symbol,
    width,
    value >> 9 & 0x7,
    value >> 6 & 0x7,
    (static_cast<int8_t>(value & 0x3F) << 2) >> 2);
#endif
}
