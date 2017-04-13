#include "../../../includes/Tokens/Directives/Token_Directive_Fill.hpp"
#include "../../../includes/Tokens/Token_Immediate.hpp"
#include "../../../includes/Tokens/Token_Label.hpp"

Fill::Fill(std::string &word, int line_number) : Directive(word, line_number)
{}

std::int32_t Fill::assemble(std::vector<std::shared_ptr<Token>> &tokens, bool *orig_seen, bool *end_seen)
{
        if (tokens.size() != 2) {
                return -1;
        }

        // TODO: Change this so it accepts labels as well (just fills that area of memory with the address
        // TODO: of the label.
        if (tokens[1]->type() != Token::IMMEDIATE && tokens[1]->type() != Token::LABEL) {
                return -1;
        }

        if (!*orig_seen) {
                expected(".ORIG directive");
                return -1;
        } else if (*end_seen) {
                WARNING(".END directive before .FILL directive, .FILL directive will be ignored.");
                return 0;
        }

        std::cout << ".FILL Found " << tokens[1]->word << '\n';

        if (!assembled.size() && tokens[1]->type() == Token::IMMEDIATE && !tokens[1]->is_error) {
                assembled.push_back(static_cast<std::uint16_t>(std::static_pointer_cast<Immediate>(tokens[1])->immediate));
        } else if (tokens[1]->type() == Token::LABEL && !tokens[1]->is_error) {
                if (assembled.size()) {
                        assembled.front() = std::static_pointer_cast<Label>(tokens[1])->address;
                } else {
                        assembled.push_back(std::static_pointer_cast<Label>(tokens[1])->address);
                }
        }

        return 1;
}

Token::token_type Fill::type() const
{
        return Token::token_type::DIR_FILL;
}
