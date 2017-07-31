#include "Tokens/Token_Directive.hpp"

Directive::Directive(std::string& directive,
                     std::string& directive_uppercase,
                     std::string& t_file,
                     size_t line_number,
                     size_t t_column)
  : Token(directive, directive_uppercase, t_file, line_number, t_column)
{
}
