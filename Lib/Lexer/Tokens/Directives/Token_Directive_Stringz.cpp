#include "Tokens/Directives/Token_Directive_Stringz.hpp"

#include <fmt/ostream.h>

Stringz::Stringz(const std::string &directive,
                 const std::string &directive_uppercase,
                 const std::string &t_file, size_t line_number, size_t t_column)
    : Directive(directive, directive_uppercase, t_file, line_number, t_column) {
}

int32_t Stringz::assemble(std::vector<std::shared_ptr<Token>> &tokens,
                          const std::map<std::string, Symbol> &symbols,
                          uint16_t program_counter) {
  (void)program_counter;
  (void)symbols;

  if (!is_valid) {
    return -1;
  }

  assembled = tokens[1]->assembled;

  return static_cast<int32_t>(assembled.size());
}

bool Stringz::valid_arguments(std::vector<std::shared_ptr<Token>> &tokens) {
  if (tokens.size() != 2) {
    invalid_argument_count(tokens.size(), 1,
                           tokens.back()->column +
                               tokens.back()->token.length());
    return (is_valid = false);
  }

  if (tokens[1]->type() != Token::_STRING) {
    tokens[1]->expected(Expected::STRING_LITERAL);
    return (is_valid = false);
  }

  if (!tokens[1]->is_valid) {
    return (is_valid = false);
  }

  return is_valid;
}

uint16_t
Stringz::guess_memory_size(std::vector<std::shared_ptr<Token>> &tokens) const {
  return static_cast<uint16_t>(is_valid) *
         static_cast<uint16_t>(tokens[1]->assembled.size());
}

std::string Stringz::disassemble(uint16_t &program_counter,
                                 const std::string &symbol, int width) const {

  const auto value = assembled.front();
#ifdef INCLUDE_ADDONS
  std::string ret =
      fmt::format("({0:04X}) {1:04X} {1:016b} ({2: >4d}) {3: "
                  "<{4}s} .FILL 0x{1:04X}\t{5:s}\n",
                  program_counter, value, line, symbol, width, file);
#else
  std::string ret = fmt::format(
      "({0:04X}) {1:04X} {1:016b} ({2: >4d}) {3: <{4}s} .FILL 0x{1:04X}\n",
      program_counter, value, line, symbol, width);
#endif

  ++program_counter;

  for (size_t index = 1; index < assembled.size(); ++index, ++program_counter) {
    const auto value = assembled[index];
#ifdef INCLUDE_ADDONS
    ret +=
        fmt::format("({0:04X}) {1:04X} {1:016b} ({2: >4d}) {3: <{4}s} .FILL "
                    "0x{1:#04X}\t{5:s}\n",
                    program_counter, value, line, std::string(), width, file);
#else
    ret += fmt::format(
        "({0:04X}) {1:04X} {1:016b} ({2: >4d}) {3: <{4}s} .FILL 0x{1:#04X}\n",
        program_counter, value, line, std::string(), width);
#endif
  }

  return ret;
}
