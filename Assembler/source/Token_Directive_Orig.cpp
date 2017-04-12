#include "../includes/Tokens/Directives/Token_Directive_Orig.hpp"

Orig::Orig(std::string &word, int line_number) : Directive(word, line_number)
{}

std::int32_t Orig::assemble(std::vector<Token *> &tokens, bool *orig_seen, bool *end_seen)
{
        if (tokens.size() != 2) {
                return -1;
        }

        if (tokens[1]->type() != Token::IMMEDIATE) {
                tokens[1]->expected("immediate value");
        }

        if (*orig_seen) {
                std::cerr << "ERROR: ";
                if (at_line) {
                        std::cerr << "Line " << at_line << ": ";
                }
                std::cerr << "Multiple .ORIG statements \n";

                is_error = true;
                return -1;
        } else if (*end_seen) {
                WARNING(".ORIG directive after .END");
        }

        origin = static_cast<std::uint16_t>(static_cast<Immediate *>(tokens[1])->immediate);
        assembled.push_back(origin);

        *orig_seen = true;
        return 0;
}

Token::token_type Orig::type() const
{
        return Token::DIR_ORIG;
}
