#include "Tokens/Token_Directive.hpp"

Directive::Directive()
        : Token()
{

}

Directive::Directive(std::string &directive, std::string &directive_uppercase, int line_number)
        : Token(directive, directive_uppercase, line_number)
{

}
