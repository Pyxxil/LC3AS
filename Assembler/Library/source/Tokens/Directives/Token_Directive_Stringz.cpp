#include "Tokens/Directives/Token_Directive_Stringz.hpp"

#include <Assembler.hpp>

Stringz::Stringz(std::string &word, int line_number)
        : Directive(word, line_number)
{}

std::int32_t Stringz::assemble(std::vector<std::shared_ptr<Token>> &tokens, Assembler &assembler)
{
        if (assembled.size()) {
                return static_cast<std::int32_t>(assembled.size());
        }

        if (tokens.size() != 2) {
                invalid_argument_count(tokens.size(), 1);
                return -1;
        }

        if (!assembler.origin_seen) {
                expected(".ORIG directive");
                return -1;
        } else if (assembler.end_seen) {
                WARNING(".END directive before .STRINGZ directive, .STRINGZ directive will be ignored.");
                return 0;
        }

        if (tokens[1]->type() != Token::_STRING) {
                tokens[1]->expected("string literal");
                return -1;
        }

        assembled = tokens[1]->assembled;

        return static_cast<std::int32_t>(assembled.size());
}

Token::token_type Stringz::type() const
{
        return Token::token_type::DIR_STRINGZ;
}
