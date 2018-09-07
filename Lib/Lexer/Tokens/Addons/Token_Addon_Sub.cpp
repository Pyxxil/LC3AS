#include "Tokens/Addons/Token_Addon_Sub.hpp"

#include <iomanip>
#include <sstream>

#include "Diagnostics.hpp"
#include "LexHelper.hpp"
#include "Tokens/Token_Register.hpp"

Sub::Sub(const std::string &directive, const std::string &directive_uppercase,
         const std::string &t_file, size_t line_number, size_t column)
    : Directive(directive, directive_uppercase, t_file, line_number, column),
      set_zero(
          std::make_shared<And>("AND", "AND", t_file, line_number, column)),
      add(std::make_shared<Add>("ADD", "ADD", t_file, line_number, column)),
      decimal_zero(
          std::make_shared<Decimal>("#0", t_file, line_number, column)),
      neg1(std::make_shared<Neg>(".NEG", ".NEG", t_file, line_number, column)),
      neg2(std::make_shared<Neg>(".NEG", ".NEG", t_file, line_number, column)) {
}

int32_t Sub::assemble(std::vector<std::shared_ptr<Token>> &tokens,
                      const std::map<std::string, Symbol> &symbols,
                      uint16_t program_counter) {
  if (!is_valid) {
    return 0;
  }

  int32_t ret = 0;

  size_t first_register_index = 1;
  size_t second_register_index = 2;

  if (tokens.size() == 4) {
    ++first_register_index;
    ++second_register_index;
  }

  if (std::static_pointer_cast<Register>(tokens[first_register_index])->reg ==
      std::static_pointer_cast<Register>(tokens[second_register_index])->reg) {
    std::vector<std::shared_ptr<Token>> vec = {
        set_zero, tokens[1], tokens[first_register_index], decimal_zero};

    set_zero->assemble(vec, symbols, program_counter);
    assembled = set_zero->assembled;

    ret = 1;
  } else {
    std::vector<std::shared_ptr<Token>> vec = {neg1,
                                               tokens[second_register_index]};
    ret += neg1->assemble(vec, symbols, program_counter);

    vec = {add, tokens[1], tokens[first_register_index],
           tokens[second_register_index]};
    ret += add->assemble(vec, symbols, program_counter);

    assembled = neg1->assembled;
    for (const auto &as_assembled : add->assembled) {
      assembled.emplace_back(as_assembled);
    }

    if (std::static_pointer_cast<Register>(tokens[1])->reg !=
        std::static_pointer_cast<Register>(tokens[second_register_index])
            ->reg) {
      vec = {neg2, tokens[second_register_index]};
      ret += neg2->assemble(vec, symbols, program_counter);

      for (const auto &as_assembled : neg2->assembled) {
        assembled.emplace_back(as_assembled);
      }
    }
  }

  return ret;
}

bool Sub::valid_arguments(std::vector<std::shared_ptr<Token>> &tokens) {
  if (tokens.size() > 4 || tokens.size() < 3) {
    invalid_argument_count(tokens.size(), 2,
                           tokens.back()->column +
                               tokens.back()->token.length());
    return (is_valid = false);
  }

  if (tokens[1]->type() != Token::REGISTER) {
    tokens[1]->expected(Expected::REGISTER);
    return (is_valid = false);
  }

  if (tokens[2]->type() != Token::REGISTER) {
    tokens[2]->expected(Expected::REGISTER);
    return (is_valid = false);
  }

  if (tokens.size() == 4) {
    if (tokens[3]->type() != Token::REGISTER) {
      tokens[3]->expected(Expected::REGISTER);
      return (is_valid = false);
    }

    if (!tokens[3]->is_valid) {
      return (is_valid = false);
    }
  }

  return (is_valid = tokens[1]->is_valid && tokens[2]->is_valid);
}

uint16_t
Sub::guess_memory_size(std::vector<std::shared_ptr<Token>> &tokens) const {
  if (is_valid) {
    size_t first_register_index = 1;
    size_t second_register_index = 2;

    if (tokens.size() == 4) {
      ++first_register_index;
      ++second_register_index;
    }

    if (std::static_pointer_cast<Register>(tokens[first_register_index])->reg ==
        std::static_pointer_cast<Register>(tokens[second_register_index])
            ->reg) {
      return 1u;
    }

    if (std::static_pointer_cast<Register>(tokens[1])->reg !=
        std::static_pointer_cast<Register>(tokens[second_register_index])
            ->reg) {
      return static_cast<uint16_t>(5);
    }

    return static_cast<uint16_t>(3);
  }

  return 0;
}

void Sub::invalid_argument_count(size_t provided, size_t expected,
                                 size_t last_column) const {
  (void)expected;
  --provided;

  std::stringstream error_string;
  error_string << ".SUB expects 2 or 3 arguments, but " << provided
               << " arguments were provided";

  Diagnostics::Diagnostic diagnostic(
      Diagnostics::FileContext(file, line, column), error_string.str(),
      Diagnostics::SYNTAX, Diagnostics::ERROR);

  if (0u != provided) {
    diagnostic.provide_context(std::make_unique<Diagnostics::HighlightContext>(
        Diagnostics::SelectionContext(
            Diagnostics::FileContext(file, line, column + token.length()), ' ',
            "Unexpected arguments found here", lexed_lines[file][line]),
        '~', last_column - (column + token.length())));
  }

  Diagnostics::push(diagnostic);
}

std::string Sub::disassemble(uint16_t &program_counter,
                             const std::string &symbol, int width) const {
  std::stringstream stream;

  if (!set_zero->assembled.empty()) {
    stream << set_zero->disassemble(program_counter, symbol, width);
  } else {
    stream << neg1->disassemble(program_counter, symbol, width);
    stream << add->disassemble(program_counter, " ", width);

    if (!neg2->assembled.empty()) {
      stream << neg2->disassemble(program_counter, " ", width);
    }
  }

  return stream.str();
}
