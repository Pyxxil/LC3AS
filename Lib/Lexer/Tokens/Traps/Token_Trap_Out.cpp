#include "Tokens/Traps/Token_Trap_Out.hpp"

#include <fmt/ostream.h>

Out::Out(const std::string &trap, const std::string &trap_uppercase,
         const std::string &t_file, size_t line_number, size_t t_column)
    : Instruction(trap, trap_uppercase, t_file, line_number, t_column) {}

int32_t Out::assemble(std::vector<std::shared_ptr<Token>> &tokens,
                      const std::map<std::string, Symbol> &symbols,
                      uint16_t program_counter) {
  (void)tokens;
  (void)symbols;
  (void)program_counter;

  if (!is_valid) {
    return -1;
  }

  assembled.emplace_back(static_cast<uint16_t>(0xF021));

  return 1;
}

bool Out::valid_arguments(std::vector<std::shared_ptr<Token>> &tokens) {
  if (tokens.size() > 1) {
    invalid_argument_count(tokens.size(), 0,
                           tokens.back()->column +
                               tokens.back()->token.length());
    return (is_valid = false);
  }

  return is_valid;
}

uint16_t
Out::guess_memory_size(std::vector<std::shared_ptr<Token>> &tokens) const {
  (void)tokens;
  return static_cast<uint16_t>(is_valid);
}

std::string Out::disassemble(uint16_t &program_counter,
                             const std::string &symbol, int width) const {
#ifdef INCLUDE_ADDONS
  return fmt::format(
      "({0:04X}) F021 1111000000100001 ({1: >4d}) {2: <{3}s} OUT\t{4:s}\n",
      program_counter++, line, symbol, width, file
  );
#else
  return fmt::format(
      "({0:04X}) F021 1111000000100001 ({1: >4d}) {2: <{3}s} OUT\n",
      program_counter++, line, symbol, width
  );
#endif
}
