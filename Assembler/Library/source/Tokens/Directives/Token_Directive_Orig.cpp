#include <Assembler.hpp>
#include "Tokens/Directives/Token_Directive_Orig.hpp"

Orig::Orig(std::string &word, int line_number)
        : Directive(word, line_number)
{}

std::int32_t Orig::assemble(std::vector<std::shared_ptr<Token>> &tokens, Assembler &assembler)
{
        if (assembled.size()) {
                assembler.origin_seen = true;
                return 0;
        }

        if (tokens.size() != 2) {
                invalid_argument_count(tokens.size(), 1);
                return -1;
        }

        if (assembler.origin_seen) {
                std::cerr << "ERROR: ";
                if (at_line) {
                        std::cerr << "Line " << std::dec << at_line << ": ";
                }
                std::cerr << "Multiple .ORIG statements\n";

                is_error = true;
                return -1;
        } else if (assembler.end_seen) {
                std::cerr << "WARNING: ";
                if (at_line) {
                        std::cerr << "Line " << std::dec << at_line << ": ";
                }
                std::cerr << ".ORIG after .END directive. It will be ignored.\n";
                return 0;
        }

        if (tokens[1]->type() != Token::IMMEDIATE) {
                tokens[1]->expected("immediate value");
        }

        origin = static_cast<std::uint16_t>(std::static_pointer_cast<Immediate>(tokens[1])->immediate);
        assembled.push_back(origin);

        assembler.origin_seen = true;
        return origin;
}

Token::token_type Orig::type() const
{
        return Token::DIR_ORIG;
}
