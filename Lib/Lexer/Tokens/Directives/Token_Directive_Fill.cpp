#include "Tokens/Directives/Token_Directive_Fill.hpp"

#include <fmt/ostream.h>

#include "Tokens/Token_Immediate.hpp"
#include "Tokens/Token_Label.hpp"

Fill::Fill(const std::string &directive, const std::string &directive_uppercase,
           const std::string &t_file, size_t line_number, size_t t_column)
    : Directive(directive, directive_uppercase, t_file, line_number, t_column) {
}

Fill::Fill(std::string &&directive, std::string &&directive_uppercase,
           const std::string &t_file, size_t line_number, size_t t_column)
    : Directive(directive, directive_uppercase, t_file, line_number, t_column) {
}

int32_t Fill::assemble(std::vector<std::shared_ptr<Token>> &tokens,
                       const std::map<std::string, Symbol> &symbols,
                       uint16_t program_counter) {
  (void)program_counter;

  if (!is_valid) {
    return 0;
  }

  if (tokens[1]->type() == Token::IMMEDIATE) {
    assembled.emplace_back(static_cast<uint16_t>(
        std::static_pointer_cast<Immediate>(tokens[1])->value));
  } else if (tokens[1]->type() == Token::LABEL) {
    if (0u == symbols.count(tokens[1]->token)) {
      std::static_pointer_cast<Label>(tokens[1])->not_found(symbols);
      return -1;
    }

    assembled.emplace_back(symbols.find(tokens[1]->token)->second.address);
  }

  return 1;
}

bool Fill::valid_arguments(std::vector<std::shared_ptr<Token>> &tokens) {
  if (tokens.size() < 2) {
    invalid_argument_count(tokens.size(), 1,
                           tokens.back()->column +
                               tokens.back()->token.length());
    return (is_valid = false);
  }

  if (tokens[1]->type() != Token::IMMEDIATE &&
      tokens[1]->type() != Token::LABEL) {
    tokens[1]->expected(Expected::LABEL_OR_IMMEDIATE_VAL);
    return (is_valid = false);
  }

  if (!tokens[1]->is_valid) {
    return (is_valid = false);
  }

  return is_valid;
}

uint16_t
Fill::guess_memory_size(std::vector<std::shared_ptr<Token>> &tokens) const {
  (void)tokens;
  return static_cast<uint16_t>(is_valid);
}

std::string Fill::disassemble(uint16_t &program_counter,
                              const std::string &symbol, int width) const {
  const auto value = static_cast<int>(assembled.front());

#ifdef INCLUDE_ADDONS
  return fmt::format("({0:04X}) {1:04X} {1:016b} ({2: >4d}) {3: <{4}s} .FILL "
                     "0x{1:04X}\t{5:s}\n",
                     program_counter++, value, line, symbol, width, file);
#else
  return fmt::format(
      "({0:04X}) {1:04X} {1:016b} ({2: >4d}) {3: <{4}s} .FILL 0x{1:04X}\n",
      program_counter++, value, line, symbol, width, value);
#endif
}
