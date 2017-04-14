#include <Assembler.hpp>
#include "Tokens/Directives/Token_Directive_Stringz.hpp"
#include "Tokens/Token_String.hpp"

Stringz::Stringz(std::string &word, int line_number)
        : Directive(word, line_number)
{}

std::int32_t Stringz::assemble(std::vector<std::shared_ptr<Token>> &tokens, Assembler &assembler)
{
        if (assembled.size()) {
                return static_cast<std::int32_t>(assembled.size());
        }

        if (tokens.size() != 2) {
                return -1;
        }

        if (tokens[1]->type() != Token::_STRING) {
                tokens[1]->expected("String literal");
                return -1;
        }

        if (!assembler.origin_seen) {
                expected(".ORIG directive");
                return -1;
        } else if (assembler.end_seen) {
                WARNING(".END directive before .STRINGZ directive, .STRINGZ directive will be ignored.");
                return 0;
        }

        for (const auto &character : std::static_pointer_cast<String>(tokens[1])->word) {
                assembled.push_back(static_cast<std::uint16_t>(character));
        }
        // Don't forget the implicit NULL character.
        assembled.push_back(0);

        return static_cast<std::int32_t>(std::static_pointer_cast<String>(tokens[1])->word.length() + 1);
}

Token::token_type Stringz::type() const
{
        return Token::token_type::DIR_STRINGZ;
}
