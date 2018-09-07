#include "Tokens/Instructions/Token_Instruction_Rti.hpp"

#include <fmt/ostream.h>

Rti::Rti(const std::string &instruction,
         const std::string &instruction_uppercase, const std::string &t_file,
         size_t line_number, size_t t_column)
    : Instruction(instruction, instruction_uppercase, t_file, line_number,
                  t_column) {}

int32_t Rti::assemble(std::vector<std::shared_ptr<Token>> &tokens,
                      const std::map<std::string, Symbol> &symbols,
                      uint16_t program_counter) {
  (void)program_counter;
  (void)tokens;
  (void)symbols;

  if (is_valid) {
    assembled.push_back(0x8000);
    return 1;
  }

  return 0;
}

uint16_t
Rti::guess_memory_size(std::vector<std::shared_ptr<Token>> &tokens) const {
  (void)tokens;

  return static_cast<uint16_t>(is_valid);
}

bool Rti::valid_arguments(std::vector<std::shared_ptr<Token>> &tokens) {
  return (is_valid = tokens.size() == 1);
}

std::string Rti::disassemble(uint16_t &program_counter,
                             const std::string &symbol, int width) const {
  const auto value = assembled.front();

#ifdef INCLUDE_ADDONS
  return fmt::format(
      "({0:04X}) {1:04X} {1:016b} ({2: >4d}) {3: <{4}s} RTI\t{5:s}\n",
      program_counter++, value, line, symbol, width, file);
#else
  return fmt::format("({0:04X}) {1:04X} {1:016b} ({2: >4d}) {3: <{4}s} RTI\n",
                     program_counter++, value, line, symbol, width);
#endif
}