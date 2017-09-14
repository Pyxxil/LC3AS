#include "Tokens/Directives/Token_Directive_Blkw.hpp"

#include <bitset>
#include <iomanip>

#include "Diagnostics.hpp"
#include "LexHelper.hpp"
#include "Tokens/Tokens.hpp"

Blkw::Blkw(const std::string& directive,
           const std::string& directive_uppercase,
           const std::string& t_file,
           size_t line_number,
           size_t t_column)
  : Directive(directive, directive_uppercase, t_file, line_number, t_column)
{}

int32_t
Blkw::assemble(std::vector<std::shared_ptr<Token>>& tokens,
               const std::map<std::string, Symbol>& symbols,
               uint16_t program_counter)
{
  (void)program_counter;

  if (!is_valid) {
    return 0;
  }

  uint16_t fill = 0;

  if (tokens.size() == 3) {
    if (tokens[2]->type() == Token::IMMEDIATE) {
      fill = static_cast<uint16_t>(
        std::static_pointer_cast<Immediate>(tokens[2])->value);
    } else if (tokens[2]->type() == Token::LABEL) {
      if (0u == symbols.count(tokens[2]->token)) {
        std::static_pointer_cast<Label>(tokens[2])->not_found(symbols);
        return -1;
      }

      fill = symbols.find(tokens[2]->token)->second.address;
    }
  }

  assembled.insert(
    assembled.end(),
    static_cast<size_t>(std::static_pointer_cast<Immediate>(tokens[1])->value),
    fill);

  return static_cast<int32_t>(assembled.size());
}

bool
Blkw::valid_arguments(std::vector<std::shared_ptr<Token>>& tokens)
{
  if (tokens.size() > 3 || tokens.size() == 1) {
    invalid_argument_count(
      tokens.size(), 1, tokens.back()->column + tokens.back()->token.length());
    return (is_valid = false);
  }

  if (tokens[1]->type() != Token::IMMEDIATE) {
    tokens[1]->expected("immediate value");
    return (is_valid = false);
  }

  if (!tokens[1]->is_valid) {
    return (is_valid = false);
  }

  if (tokens.size() == 3) {
    if (tokens[2]->type() != Token::IMMEDIATE &&
        tokens[2]->type() != Token::LABEL) {
      tokens[2]->expected("label or immediate value");
      return (is_valid = false);
    }

    if (!tokens[2]->is_valid) {
      return (is_valid = false);
    }
  }

  return is_valid;
}

uint16_t
Blkw::guess_memory_size(std::vector<std::shared_ptr<Token>>& tokens) const
{
  return static_cast<uint16_t>(is_valid) *
         static_cast<uint16_t>(
           std::static_pointer_cast<Immediate>(tokens[1])->value);
}

void
Blkw::invalid_argument_count(size_t provided,
                             size_t expected,
                             size_t last_column) const
{
  (void)expected;

  --provided;

  std::stringstream error_string;
  error_string << ".BLKW expects 1 or 2 arguments, but " << provided
               << " arguments were provided";

  Diagnostics::Diagnostic diagnostic(
    Diagnostics::FileContext(file, line, column),
    error_string.str(),
    Diagnostics::SYNTAX,
    Diagnostics::ERROR);

  if (0u != provided) {
    diagnostic.provide_context(std::make_unique<Diagnostics::HighlightContext>(
      Diagnostics::SelectionContext(
        Diagnostics::FileContext(file, line, column + token.length()),
        ' ',
        "Unexpected arguments found here",
        lexed_lines[file][line]),
      '~',
      last_column - (column + token.length())));
  }

  Diagnostics::push(diagnostic);
}

std::string
Blkw::disassemble(uint16_t& program_counter,
                  const std::string& symbol,
                  int width) const
{
  std::stringstream stream;

  auto value = static_cast<int>(assembled.front());

  stream
    // Address in memory
    << '(' << std::hex << std::uppercase << std::setfill('0') << std::setw(4)
    << program_counter
    << ')'
    // Hexadecimal representation of instruction
    << ' ' << std::hex << std::setfill('0') << std::setw(4)
    << value
    // Binary representation of instruction
    << ' '
    << std::bitset<16>(static_cast<unsigned long long>(value))
    // Line the instruction is on
    << " (" << std::setfill(' ') << std::right << std::dec << std::setw(4)
    << line
    << ')'
    // Label at the current address (if any)
    << ' ' << std::left << std::setfill(' ') << std::setw(width)
    << symbol
    // Instruction itself
    << " .FILL 0x" << std::right << std::hex << std::setfill('0')
    << std::setw(4) << value
#ifdef INCLUDE_ADDONS
    << '\t' << file
#endif
    << '\n';

  ++program_counter;

  for (size_t block = 1; block < assembled.size(); ++block, ++program_counter) {
    stream
      // Address in memory
      << '(' << std::hex << std::uppercase << std::setfill('0') << std::setw(4)
      << program_counter
      << ')'
      // Hexadecimal representation of instruction
      << ' ' << std::hex << std::setfill('0') << std::setw(4)
      << value
      // Binary representation of instruction
      << ' '
      << std::bitset<16>(static_cast<unsigned long long>(value))
      // Line the instruction is on
      << " (" << std::setfill(' ') << std::right << std::dec << std::setw(4)
      << line
      << ')'
      // Label at the current address (if any)
      << ' ' << std::setfill(' ') << std::setw(width)
      << ' '
      // Instruction itself
      << " .FILL 0x" << std::right << std::hex << std::setfill('0')
      << std::setw(4) << value
#ifdef INCLUDE_ADDONS
      << '\t' << file
#endif
      << '\n';
  }

  return stream.str();
}
