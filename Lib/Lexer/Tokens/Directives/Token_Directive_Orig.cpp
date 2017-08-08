#include "Tokens/Directives/Token_Directive_Orig.hpp"

#include <bitset>
#include <iomanip>
#include <sstream>

Orig::Orig(std::string& directive,
           std::string& directive_uppercase,
           std::string& t_file,
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
    std::static_pointer_cast<Immediate>(tokens.at(1))->value);
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

  if (tokens.at(1)->type() != Token::IMMEDIATE) {
    tokens.at(1)->expected("immediate value");
    return (is_valid = false);
  }

  if (!tokens.at(1)->is_valid) {
    return (is_valid = false);
  }

  return is_valid;
}

uint16_t
Orig::guess_memory_size(std::vector<std::shared_ptr<Token>>& tokens) const
{
  if (is_valid) {
    return static_cast<uint16_t>(static_cast<uint16_t>(
      std::static_pointer_cast<Immediate>(tokens.at(1))->value));
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

  std::stringstream stream;
  stream
    // Address in memory
    << "(0000) "
    << std::uppercase
    // Hexadecimal representation of instruction
    << std::hex << std::setfill('0') << std::setw(4)
    << program_counter
    // Binary representation of instruction
    << ' '
    << std::bitset<16>(program_counter)
    // Line the instruction is on
    << " (" << std::setfill(' ') << std::right << std::dec << std::setw(4)
    << line
    << ')'
    // Label at the current address (if any)
    << ' ' << std::setfill(' ') << std::setw(width)
    << ' '
    // Instruction itself
    << ' ' << token_uppercase << " 0x" << std::hex << std::setfill('0')
    << std::setw(4) << program_counter
#ifdef INCLUDE_ADDONS
    << '\t' << file
#endif
    << '\n';
  ;
  return stream.str();
}

Token::token_type
Orig::type() const
{
  return Token::DIR_ORIG;
}