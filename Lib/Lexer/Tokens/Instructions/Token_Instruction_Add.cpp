#include "Tokens/Instructions/Token_Instruction_Add.hpp"

#include <fmt/ostream.h>

#include "Tokens/Token_Immediate.hpp"
#include "Tokens/Token_Register.hpp"

Add::Add(const std::string &instruction,
         const std::string &instruction_uppercase, const std::string &t_file,
         size_t line_number, size_t t_column)
    : Instruction(instruction, instruction_uppercase, t_file, line_number,
                  t_column) {}

Add::Add(std::string &&instruction, std::string &&instruction_uppercase,
         const std::string &t_file, size_t line_number, size_t t_column)
    : Instruction(instruction, instruction_uppercase, t_file, line_number,
                  t_column) {}

int32_t Add::assemble(std::vector<std::shared_ptr<Token>> &tokens,
                      const std::map<std::string, Symbol> &symbols,
                      uint16_t program_counter) {
  (void)symbols;
  (void)program_counter;

  if (!is_valid) {
    return 0;
  }

  assembled.emplace_back(static_cast<uint16_t>(
      0x1000 | (std::static_pointer_cast<Register>(tokens[1])->reg << 9)));

  if (tokens.size() == 4) { // We have a normal ADD
    if (tokens[3]->type() == Token::REGISTER) {
      assembled.front() |= static_cast<uint16_t>(
          std::static_pointer_cast<Register>(tokens[3])->reg);
    } else {
      assembled.front() |=
          0x20u | (static_cast<uint16_t>(
                       std::static_pointer_cast<Immediate>(tokens[3])->value) &
                   0x1Fu);
    }
  } else { // We have something like ADD R1, #15 or ADD R1, R2
    assembled.front() |= static_cast<uint16_t>(
        std::static_pointer_cast<Register>(tokens[1])->reg << 6);
    if (tokens[2]->type() == Token::REGISTER) {
      assembled.front() |= static_cast<uint16_t>(
          std::static_pointer_cast<Register>(tokens[2])->reg);
    } else {
      assembled.front() |=
          0x20u | (static_cast<uint16_t>(
                       std::static_pointer_cast<Immediate>(tokens[2])->value) &
                   0x1Fu);
    }
  }

  return 1;
}

bool Add::valid_arguments(std::vector<std::shared_ptr<Token>> &tokens) {
  if (tokens.size() != 4 && tokens.size() != 3) {
    invalid_argument_count(tokens.size(), 3,
                           tokens.back()->column +
                               tokens.back()->token.length());
    return (is_valid = false);
  }

  if (tokens[1]->type() != Token::REGISTER) {
    tokens[1]->expected("register");
    return (is_valid = false);
  }

  if (tokens[2]->type() != Token::REGISTER &&
      (tokens.size() == 3 && tokens[2]->type() != Token::IMMEDIATE)) {
    tokens[2]->expected("register or immediate value");
    return (is_valid = false);
  }

  if (tokens.size() == 4 && (tokens[3]->type() != Token::REGISTER &&
                             tokens[3]->type() != Token::IMMEDIATE)) {
    tokens[3]->expected("register or immediate value");
    return (is_valid = false);
  }

  if (tokens.size() == 4 && tokens[3]->type() == Token::IMMEDIATE) {
    if (std::static_pointer_cast<Immediate>(tokens[3])->value > 15 ||
        std::static_pointer_cast<Immediate>(tokens[3])->value < -16) {
      tokens[3]->requires_too_many_bits(5, SIGNED, this,
                                        std::map<std::string, Symbol>());
      return (is_valid = false);
    }
  }

  return !(tokens[1]->is_valid && tokens[2]->is_valid &&
           ((tokens.size() == 4 && tokens[3]->is_valid) || tokens.size() == 3))
             ? (is_valid = false)
             : is_valid;
}

uint16_t
Add::guess_memory_size(std::vector<std::shared_ptr<Token>> &tokens) const {
  (void)tokens;
  return static_cast<uint16_t>(is_valid);
}

std::string Add::disassemble(uint16_t &program_counter,
                             const std::string &symbol, int width) const {
  const auto value = assembled.front();

#ifdef INCLUDE_ADDONS
  return fmt::format(
      "({0:04X}) {1:04X} {1:016b} ({2: >4d}) {3: <{4}s} ADD R{5:d} R{6:d} "
      "{7:s}\t{8:s}\n",
      program_counter++, value, line, symbol, width, value >> 9 & 0x7,
      value >> 6 & 0x7,
      value & 0x20
          ? fmt::format(
                "#{:d}",
                (static_cast<int8_t>(
                     static_cast<std::int8_t>(assembled.front() & 0x1F) << 3) >>
                 3))
          : fmt::format("R{:d}", value & 7),
      file);
#else
  return fmt::format(
      "({0:04X}) {1:04X} {1:016b} ({2: >4d}) {3: <{4}s} ADD R{5:d} R{6:d} "
      "{7:s}\n",
      program_counter++, value, line, symbol, width, value >> 9 & 0x7,
      value >> 6 & 0x7,
      value & 0x20
          ? fmt::format(
                "#{:d}",
                (static_cast<int8_t>(
                     static_cast<std::int8_t>(assembled.front() & 0x1F) << 3) >>
                 3))
          : fmt::format("R{:d}", value & 7));
#endif
}
