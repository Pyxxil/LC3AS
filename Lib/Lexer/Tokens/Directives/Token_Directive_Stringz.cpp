#include "Tokens/Directives/Token_Directive_Stringz.hpp"

#include <bitset>
#include <iomanip>
#include <sstream>

Stringz::Stringz(std::string& directive,
                 std::string& directive_uppercase,
                 std::string& t_file,
                 size_t line_number,
                 size_t t_column)
  : Directive(directive, directive_uppercase, t_file, line_number, t_column)
{}

int32_t
Stringz::assemble(std::vector<std::shared_ptr<Token>>& tokens,
                  const std::map<std::string, Symbol>& symbols,
                  uint16_t program_counter)
{
  (void)program_counter;
  (void)symbols;

  if (!is_valid) {
    return -1;
  }

  assembled = tokens.at(1)->assembled;

  return static_cast<int32_t>(assembled.size());
}

bool
Stringz::valid_arguments(std::vector<std::shared_ptr<Token>>& tokens)
{
  if (tokens.size() != 2) {
    invalid_argument_count(
      tokens.size(), 1, tokens.back()->column + tokens.back()->token.length());
    return (is_valid = false);
  }

  if (tokens.at(1)->type() != Token::_STRING) {
    tokens.at(1)->expected("string literal");
    return (is_valid = false);
  }

  if (!tokens.at(1)->is_valid) {
    return (is_valid = false);
  }

  return is_valid;
}

uint16_t
Stringz::guess_memory_size(std::vector<std::shared_ptr<Token>>& tokens) const
{
  return static_cast<uint16_t>(is_valid) *
         static_cast<uint16_t>(tokens.at(1)->assembled.size());
}

std::string
Stringz::disassemble(uint16_t& program_counter,
                     const std::string& symbol,
                     int width) const
{
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
    << std::bitset<16>(static_cast<unsigned long long>(assembled.front()))
    // Line the instruction is on
    << " (" << std::setfill(' ') << std::right << std::dec << std::setw(4)
    << line
    << ')'
    // Label at the current address (if any)
    << ' ' << std::left << std::setfill(' ') << std::setw(width)
    << symbol
    // Instruction itself
    << " .FILL 0x" << std::hex << std::setfill('0') << std::setw(4)
    << assembled.front()
#ifdef INCLUDE_ADDONS
    << '\t' << file
#endif
    << '\n';

  ++program_counter;

  for (size_t index = 1; index != assembled.size(); ++index) {
    stream
      // Address in memory
      << '(' << std::hex << std::uppercase << std::setfill('0') << std::setw(4)
      << program_counter
      << ')'
      // Hexadecimal representation of instruction
      << ' ' << std::hex << std::setfill('0') << std::setw(4)
      << assembled.at(index)
      // Binary representation of instruction
      << ' '
      << std::bitset<16>(static_cast<unsigned long long>(assembled.at(index)))
      // Line the instruction is on
      << " (" << std::setfill(' ') << std::right << std::dec << std::setw(4)
      << line
      << ')'
      // Label at the current address (if any)
      << ' ' << std::setfill(' ') << std::setw(width)
      << ' '
      // Instruction itself
      << " .FILL 0x" << std::hex << std::setfill('0') << std::setw(4)
      << assembled.at(index)
#ifdef INCLUDE_ADDONS
      << '\t' << file
#endif
      << '\n';

    ++program_counter;
  }

  return stream.str();
}

Token::token_type
Stringz::type() const
{
  return Token::token_type::DIR_STRINGZ;
}