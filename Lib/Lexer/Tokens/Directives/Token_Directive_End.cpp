#include "Tokens/Directives/Token_Directive_End.hpp"

#include <fmt/ostream.h>

End::End(const std::string& directive,
         const std::string& directive_uppercase,
         const std::string& t_file,
         size_t line_number,
         size_t t_column)
  : Directive(directive, directive_uppercase, t_file, line_number, t_column)
{}

int32_t
End::assemble(std::vector<std::shared_ptr<Token>>& tokens,
              const std::map<std::string, Symbol>& symbols,
              uint16_t program_counter)
{
  (void)tokens;
  (void)symbols;
  (void)program_counter;

  return 0;
}

bool
End::valid_arguments(std::vector<std::shared_ptr<Token>>& tokens)
{
  if (tokens.size() > 1) {
    invalid_argument_count(
      tokens.size(), 0, tokens.back()->column + tokens.back()->token.length());
    return (is_valid = false);
  }

  return is_valid;
}

uint16_t
End::guess_memory_size(std::vector<std::shared_ptr<Token>>& tokens) const
{
  (void)tokens;
  return 0;
}

std::string
End::disassemble(uint16_t& program_counter,
                 const std::string& symbol,
                 int width) const
{
  (void)symbol;

#ifdef INCLUDE_ADDONS
  return fmt::format("({0:04X}) 0000 0000000000000000 ({1: >4d}) {2: <{3}s} "
                     ".END\t{4:s}\n",
                     program_counter++,
                     line,
                     std::string(),
                     width,
                     file);
#else
  return fmt::format("({0:04X}) 0000 0000000000000000 ({1: >4d}) {2: <{3}s} "
                     ".END\n",
                     program_counter++,
                     line,
                     std::string(),
                     width);
#endif
}
