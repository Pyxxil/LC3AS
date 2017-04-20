#include <Assembler.hpp>
#include "Tokens/Directives/Token_Directive_Orig.hpp"

Orig::Orig(std::string &token, int line_number)
        : Directive(token, line_number)
{}

std::int32_t Orig::assemble(std::vector<std::shared_ptr<Token>> &tokens, Assembler &assembler)
{
        if (!is_valid) {
                return -1;
        }

        origin = static_cast<std::uint16_t>(std::static_pointer_cast<Immediate>(tokens[1])->immediate);
        assembled.emplace_back(origin);

        assembler.origin_seen = true;
        return origin;
}

bool Orig::valid_arguments(std::vector<std::shared_ptr<Token>> &tokens)
{
        if (tokens.size() != 2) {
                invalid_argument_count(tokens.size(), 1);
                return (is_valid = false);
        }

        if (tokens.at(1)->type() != Token::IMMEDIATE) {
                tokens.at(1)->expected("immediate value");
                return (is_valid = false);
        } else if (!tokens.at(1)->is_valid) {
                return (is_valid = false);
        }

        return is_valid;
}

std::int32_t Orig::guess_memory_size(std::vector<std::shared_ptr<Token>> &tokens) const
{
        if (is_valid) {
                return static_cast<std::int32_t>(std::static_pointer_cast<Immediate>(tokens.at(1))->immediate);
        } else {
                return -1;
        }
}

Token::token_type Orig::type() const
{
        return Token::DIR_ORIG;
}
