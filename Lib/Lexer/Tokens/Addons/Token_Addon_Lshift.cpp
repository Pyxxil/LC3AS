#include "Tokens/Addons/Token_Addon_Lshift.hpp"

#include <sstream>

#include "Tokens/Tokens.hpp"

Lshift::Lshift(const std::string& directive,
               const std::string& directive_uppercase,
               const std::string& t_file,
               size_t line_number,
               size_t column)
  : Directive(directive, directive_uppercase, t_file, line_number, column)
  , add(std::make_shared<Add>("ADD", "ADD", t_file, line_number, column))
{}

int32_t
Lshift::assemble(std::vector<std::shared_ptr<Token>>& tokens,
                 const std::map<std::string, Symbol>& symbols,
                 uint16_t program_counter)
{
  std::vector<std::shared_ptr<Token>> vec = {
    add, tokens[1], tokens[1], tokens[1]
  };

  add->assemble(vec, symbols, program_counter);

  const uint16_t machine_instruction = add->assembled.front();

  assembled.insert(assembled.end(),
                   static_cast<size_t>(
                     std::static_pointer_cast<Immediate>(tokens[2])->value),
                   machine_instruction);

  return static_cast<int32_t>(assembled.size());
}

bool
Lshift::valid_arguments(std::vector<std::shared_ptr<Token>>& tokens)
{
  if (tokens.size() != 3) {
    invalid_argument_count(
      tokens.size(), 2, tokens.back()->column + tokens.back()->token.length());
    return (is_valid = false);
  }

  if (tokens[1]->type() != Token::REGISTER) {
    tokens[1]->expected("register");
    return (is_valid = false);
  }

  if (!tokens[1]->is_valid) {
    return (is_valid = false);
  }

  if (tokens[2]->type() != Token::IMMEDIATE) {
    tokens[2]->expected("immediate value");
    return (is_valid = false);
  }

  if (!tokens[2]->is_valid) {
    return (is_valid = false);
  }

  if (std::static_pointer_cast<Immediate>(tokens[2])->value > 15 ||
      std::static_pointer_cast<Immediate>(tokens[2])->value < 1) {
    tokens[2]->expected("4 bit unsigned value between 1 and 15 (inclusive)");
    return (is_valid = false);
  }

  return is_valid;
}

uint16_t
Lshift::guess_memory_size(std::vector<std::shared_ptr<Token>>& tokens) const
{
  if (!is_valid) {
    return 0;
  }

  return static_cast<uint16_t>(
    std::static_pointer_cast<Immediate>(tokens[2])->value);
}

std::string
Lshift::disassemble(uint16_t& program_counter,
                    const std::string& symbol,
                    int width) const
{
  std::stringstream stream;

  stream << add->disassemble(program_counter, symbol, width);
  const std::string& disassembled_without_symbol =
    add->disassemble(program_counter, "", width);

  // The previous instruction will increment this. We want it to be it's
  // original value, as we increment it ourselves in the loop below.
  --program_counter;

  for (size_t shift = 1; shift < assembled.size(); ++shift) {
    stream << disassembled_without_symbol;
    ++program_counter;
  }

  return stream.str();
}

Token::token_type
Lshift::type() const
{
  return Token::ADDON_LSHIFT;
}
