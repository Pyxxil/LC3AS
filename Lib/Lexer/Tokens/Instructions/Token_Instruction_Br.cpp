#include "Tokens/Instructions/Token_Instruction_Br.hpp"

#include <bitset>
#include <iomanip>
#include <sstream>

#include "Tokens/Token_Immediate.hpp"
#include "Tokens/Tokens.hpp"

Br::Br(const std::string &instruction, const std::string &instruction_uppercase,
       const std::string &t_file, size_t line_number, size_t t_column, bool n,
       bool z, bool p)
    : Instruction(instruction, instruction_uppercase, t_file, line_number,
                  t_column),
      N(n), Z(z), P(p), provided() {}

Br::Br(std::string &&instruction, std::string &&instruction_uppercase,
       const std::string &t_file, size_t line_number, size_t t_column, bool n,
       bool z, bool p)
    : Instruction(instruction, instruction_uppercase, t_file, line_number,
                  t_column),
      N(n), Z(z), P(p), provided() {}

int32_t Br::assemble(std::vector<std::shared_ptr<Token>> &tokens,
                     const std::map<std::string, Symbol> &symbols,
                     uint16_t program_counter) {
  if (!is_valid) {
    return 0;
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

  if (offset > 255 || offset < -256) {
    tokens[1]->requires_too_many_bits(9, SIGNED, this, symbols);
    return -1;
  }

  provided = tokens[1];
  assembled.emplace_back(static_cast<uint16_t>(
      0x0000 | static_cast<int>(N) << 11 | static_cast<int>(Z) << 10 |
      static_cast<int>(P) << 9 | (offset & 0x1FF)));

  return 1;
}

bool Br::valid_arguments(std::vector<std::shared_ptr<Token>> &tokens) {

  if (tokens.size() != 2) {
    invalid_argument_count(tokens.size(), 1,
                           tokens.back()->column +
                               tokens.back()->token.length());
    return (is_valid = false);
  }

  if (tokens[1]->type() != Token::LABEL &&
      tokens[1]->type() != Token::IMMEDIATE) {
    tokens[1]->expected("label or immediate value");
    return (is_valid = false);
  }

  if (!tokens[1]->is_valid) {
    return (is_valid = false);
  }

  return is_valid;
}

uint16_t
Br::guess_memory_size(std::vector<std::shared_ptr<Token>> &tokens) const {
  (void)tokens;
  return static_cast<uint16_t>(is_valid);
}

std::string Br::disassemble(uint16_t &program_counter,
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
      << " BR";

  if (N) {
    stream << 'n';
  }

  if (Z) {
    stream << 'z';
  }
  if (P) {
    stream << 'p';
  }

  ++program_counter;

  if (provided->type() == Token::LABEL) {
    stream << ' ' << provided->token;
  } else {
    const auto offset = std::static_pointer_cast<Immediate>(provided)->value;
    stream << " 0x" << std::hex << std::setfill('0') << std::setw(4)
           << (offset + program_counter);
  }

#ifdef INCLUDE_ADDONS
  stream << '\t' << file;
#endif
  stream << '\n';

  return stream.str();
}
