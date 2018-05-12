#include "Tokens/Instructions/Token_Instruction_Not.hpp"

#include <bitset>
#include <iomanip>
#include <sstream>

#include "Tokens/Token_Register.hpp"

Not::Not(const std::string &instruction,
         const std::string &instruction_uppercase, const std::string &t_file,
         size_t line_number, size_t t_column)
    : Instruction(instruction, instruction_uppercase, t_file, line_number,
                  t_column) {}

Not::Not(std::string &&instruction, std::string &&instruction_uppercase,
         const std::string &t_file, size_t line_number, size_t t_column)
    : Instruction(instruction, instruction_uppercase,
                  t_file, line_number, t_column) {}

int32_t Not::assemble(std::vector<std::shared_ptr<Token>> &tokens,
                      const std::map<std::string, Symbol> &symbols,
                      uint16_t program_counter) {
  (void)symbols;
  (void)program_counter;

  if (!is_valid) {
    return -1;
  }

  assembled.emplace_back(static_cast<uint16_t>(
      0x903F |
      ((std::static_pointer_cast<Register>(tokens[1])->reg & 0x7) << 9) |
      ((std::static_pointer_cast<Register>(tokens[2])->reg & 0x7) << 6)));

  return 1;
}

bool Not::valid_arguments(std::vector<std::shared_ptr<Token>> &tokens) {
  if (tokens.size() != 3) {
    invalid_argument_count(tokens.size(), 2,
                           tokens.back()->column +
                               tokens.back()->token.length());
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

  if (!(tokens[1]->is_valid && tokens[2]->is_valid)) {
    return (is_valid = false);
  }

  return is_valid;
}

uint16_t
Not::guess_memory_size(std::vector<std::shared_ptr<Token>> &tokens) const {
  (void)tokens;
  return static_cast<uint16_t>(is_valid);
}

std::string Not::disassemble(uint16_t &program_counter,
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
      << " NOT R" << ((assembled.front() & 0x0E00) >> 9 & 7) << " R"
      << ((assembled.front() & 0x01C0) >> 6 & 7)
#ifdef INCLUDE_ADDONS
      << '\t' << file
#endif
      << '\n';

  ++program_counter;

  return stream.str();
}
