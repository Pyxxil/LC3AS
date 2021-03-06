#include "Tokens/Addons/Token_Addon_Neg.hpp"

#include <iomanip>
#include <sstream>

#include "Tokens/Immediates/Token_Immediate_Decimal.hpp"

Neg::Neg(const std::string &directive, const std::string &directive_uppercase,
         const std::string &t_file, size_t line_number, size_t column)
    : Directive(directive, directive_uppercase, t_file, line_number, column),
      neg(std::make_shared<Not>("NOT", "NOT", t_file, line_number, column)),
      add(std::make_shared<Add>("ADD", "ADD", t_file, line_number, column)) {}

Neg::Neg(std::string &&directive, std::string &&directive_uppercase,
         const std::string &t_file, size_t line_number, size_t column)
    : Directive(directive, directive_uppercase, t_file, line_number, column),
      neg(std::make_shared<Not>("NOT", "NOT", t_file, line_number, column)),
      add(std::make_shared<Add>("ADD", "ADD", t_file, line_number, column)) {}

int32_t Neg::assemble(std::vector<std::shared_ptr<Token>> &tokens,
                      const std::map<std::string, Symbol> &symbols,
                      uint16_t program_counter) {
  std::vector<std::shared_ptr<Token>> vec = {neg, tokens[1], tokens[1]};

  neg->assemble(vec, symbols, program_counter);
  vec = {add, tokens[1], tokens[1],
         std::make_shared<Decimal>("#1", file, line, column)};
  add->assemble(vec, symbols, program_counter);

  assembled = neg->assembled;
  for (const auto &as_assembled : add->assembled) {
    assembled.emplace_back(as_assembled);
  }

  return 2;
}

bool Neg::valid_arguments(std::vector<std::shared_ptr<Token>> &tokens) {
  if (tokens.size() != 2) {
    invalid_argument_count(tokens.size(), 1,
                           tokens.back()->column +
                               tokens.back()->token.length());
    return (is_valid = false);
  }

  if (tokens[1]->type() != Token::REGISTER) {
    tokens[1]->expected(Expected::REGISTER);
    return (is_valid = false);
  }

  return is_valid;
}

uint16_t
Neg::guess_memory_size(std::vector<std::shared_ptr<Token>> &tokens) const {
  (void)tokens;
  return static_cast<uint16_t>(static_cast<uint16_t>(is_valid) * 2);
}

std::string Neg::disassemble(uint16_t &program_counter,
                             const std::string &symbol, int width) const {
  std::stringstream stream;

  stream << neg->disassemble(program_counter, symbol, width);
  stream << add->disassemble(program_counter, " ", width);

  return stream.str();
}
