#include <Assembler.hpp>
#include "Tokens/Directives/Token_Directive_End.hpp"

End::End(std::string &token, int line_number)
        : Directive(token, line_number)
{}

std::int32_t End::assemble(std::vector<std::shared_ptr<Token>> &tokens, Assembler &assembler)
{
        if (tokens.size() > 1) {
                invalid_argument_count(tokens.size(), 0);
                return -1;
        }

        if (!assembler.origin_seen) {
                expected(".ORIG directive");
                return -1;
        } else if (assembler.end_seen) {
                std::cerr << "WARNING: ";
                if (at_line) {
                        std::cerr << "Line " << std::dec << at_line << ": ";
                }
                std::cerr << "Multiple .END statements\n";
                is_error = true;
        }

        assembler.end_seen = true;
        return 0;
}

Token::token_type End::type() const
{
        return Token::token_type::DIR_END;
}
