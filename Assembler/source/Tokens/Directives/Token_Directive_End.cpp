#include "../../../includes/Tokens/Directives/Token_Directive_End.hpp"

End::End(std::string &word, int line_number) : Directive(word, line_number)
{}

std::int32_t End::assemble(std::vector<std::shared_ptr<Token>> &tokens, bool *orig_seen, bool *end_seen)
{
        if (tokens.size() > 1) {
                return -1;
        }

        if (!*orig_seen) {
                expected(".ORIG directive");
                return -1;
        } else if (*end_seen) {
                std::cerr << "ERROR: ";
                if (at_line) {
                        std::cerr << "Line " << at_line << ": ";
                }
                WARNING("Multiple .END statements");
                is_error = true;
        }

        *end_seen = true;
        return 0;
}

Token::token_type End::type() const
{
        return Token::token_type::DIR_END;
}
