#include "Tokens/Token_Directive.hpp"

Directive::Directive()
        : Token()
{

}

Directive::Directive(std::string &directive,
                     std::string &directive_uppercase,
                     std::string &t_file,
                     int line_number)
        : Token(directive, directive_uppercase, t_file, line_number)
{

}
