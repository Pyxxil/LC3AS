#include "Tokens/Instructions/Token_Instruction_Rti.hpp"

#include <bitset>
#include <iomanip>
#include <sstream>

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
  std::stringstream stream;
  stream
      // Address in memory
      << '(' << std::hex << std::uppercase << std::setfill('0') << std::setw(4)
      << program_counter
      << ')'
      // Hexadecimal representation of instruction
      << ' ' << std::hex << std::setfill('0') << std::setw(4)
      << assembled.front()
      // Binary representation of instruction
      << ' '
      << std::bitset<16>(assembled.front())
      // Line the instruction is on
      << " (" << std::setfill(' ') << std::right << std::dec << std::setw(4)
      << line
      << ')'
      // Label at the current address (if any)
      << ' ' << std::left << std::setfill(' ') << std::setw(width)
      << symbol
      // Instruction itself
      << " RTI";

#ifdef INCLUDE_ADDONS
  stream << '\t' << file;
#endif
  stream << '\n';

  ++program_counter;

  return stream.str();
}