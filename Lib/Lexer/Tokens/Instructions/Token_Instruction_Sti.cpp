#include "Tokens/Instructions/Token_Instruction_Sti.hpp"

#include <fmt/ostream.h>

#include "Tokens/Token_Immediate.hpp"
#include "Tokens/Token_Label.hpp"
#include "Tokens/Token_Register.hpp"

Sti::Sti(const std::string &instruction,
         const std::string &instruction_uppercase, const std::string &t_file,
         size_t line_number, size_t t_column)
    : Instruction(instruction, instruction_uppercase, t_file, line_number,
                  t_column) {}

int32_t Sti::assemble(std::vector<std::shared_ptr<Token>> &tokens,
                      const std::map<std::string, Symbol> &symbols,
                      uint16_t program_counter) {
  if (!is_valid) {
    return 0;
  }

  int offset = 0;

  if (tokens[2]->type() == Token::LABEL) {
    if (0u == symbols.count(tokens[2]->token)) {
      std::static_pointer_cast<Label>(tokens[2])->not_found(symbols);
      return -1;
    }

    offset = static_cast<int>(symbols.find(tokens[2]->token)->second.address) -
             (static_cast<int>(program_counter) + 1);
  } else {
    offset = std::static_pointer_cast<Immediate>(tokens[2])->value;
  }

  if (offset > 255 || offset < -256) {
    tokens[2]->requires_too_many_bits(9, SIGNED, this, symbols);
    return -1;
  }

  provided = tokens[2];

  assembled.emplace_back(static_cast<uint16_t>(
      0xB000 | ((std::static_pointer_cast<Register>(tokens[1])->reg & 7) << 9) |
      (offset & 0x1FF)));

  return 1;
}

bool Sti::valid_arguments(std::vector<std::shared_ptr<Token>> &tokens) {
  if (tokens.size() != 3) {
    invalid_argument_count(tokens.size(), 2,
                           tokens.back()->column +
                               tokens.back()->token.length());
    return (is_valid = false);
  }

  if (tokens[1]->type() != Token::REGISTER) {
    tokens[1]->expected(Expected::REGISTER);
    return (is_valid = false);
  }

  if (tokens[2]->type() != Token::LABEL &&
      tokens[2]->type() != Token::IMMEDIATE) {
    tokens[2]->expected(Expected::LABEL_OR_IMMEDIATE_VAL);
    return (is_valid = false);
  }

  if (!(tokens[1]->is_valid && tokens[2]->is_valid)) {
    return (is_valid = false);
  }

  return is_valid;
}

uint16_t
Sti::guess_memory_size(std::vector<std::shared_ptr<Token>> &tokens) const {
  (void)tokens;
  return static_cast<uint16_t>(is_valid);
}

std::string Sti::disassemble(uint16_t &program_counter,
                             const std::string &symbol, int width) const {
  const auto value = assembled.front();

  ++program_counter;

#ifdef INCLUDE_ADDONS
  return fmt::format(
      "({0:04X}) {1:04X} {1:016b} ({2: >4d}) {3: <{4}s} STI R{5:d} "
      "{6:s}\t{7:s}\n",
      program_counter - 1, value, line, symbol, width, value >> 9 & 0x7,
      provided->type() == Token::LABEL
          ? provided->token
          : fmt::format("{:#06x}",
                        std::static_pointer_cast<Immediate>(provided)->value +
                            program_counter),
      file);
#else
  return fmt::format(
      "({0:04X}) {1:04X} {1:016b} ({2: >4d}) {3: <{4}s} STI R{5:d} {6:s}\n",
      program_counter - 1, value, line, symbol, width, value >> 9 & 0x7,
      provided->type() == Token::LABEL
          ? provided->token
          : fmt::format("{:#06x}",
                        std::static_pointer_cast<Immediate>(provided)->value +
                            program_counter));
#endif
}
