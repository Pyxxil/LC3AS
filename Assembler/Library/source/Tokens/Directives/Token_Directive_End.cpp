#include <Assembler.hpp>
#include "Tokens/Directives/Token_Directive_End.hpp"

End::End(std::string &token, int line_number)
        : Directive(token, line_number)
{}

std::int32_t End::assemble(std::vector<std::shared_ptr<Token>> &tokens, Assembler &assembler)
{
        (void) tokens;

        assembler.end_seen = true;
        return 0;
}

bool End::valid_arguments(std::vector<std::shared_ptr<Token>> &tokens)
{
        if (tokens.size() > 1) {
                invalid_argument_count(tokens.size(), 0);
                return (is_valid = false);
        }

        return is_valid;
}

std::int32_t End::guess_memory_size(std::vector<std::shared_ptr<Token>> &tokens) const
{
        (void) tokens;
        return 0;
}

Token::token_type End::type() const
{
        return Token::token_type::DIR_END;
}
