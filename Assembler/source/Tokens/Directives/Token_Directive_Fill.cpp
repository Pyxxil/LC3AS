#include "Tokens/Directives/Token_Directive_Fill.hpp"
#include "Tokens/Token_Immediate.hpp"
#include "Tokens/Token_Label.hpp"

#include <Assembler.hpp>

Fill::Fill(std::string &word, int line_number) : Directive(word, line_number), first_time(true)
{}

std::int32_t Fill::assemble(std::vector<std::shared_ptr<Token>> &tokens, Assembler &assembler)
{
        if (tokens.size() != 2) {
                return -1;
        }

        if (!assembler.origin_seen) {
                expected(".ORIG directive");
                return -1;
        } else if (assembler.end_seen) {
                WARNING(".END directive before .FILL directive, .FILL directive will be ignored.");
                return 0;
        }

        if (tokens[1]->type() != Token::IMMEDIATE && tokens[1]->type() != Token::LABEL) {
                tokens[1]->expected("valid immediate value or label");
                return -1;
        }

        if (!assembled.size() && tokens[1]->type() == Token::IMMEDIATE && !tokens[1]->is_error) {
                assembled.push_back(
                        static_cast<std::uint16_t>(std::static_pointer_cast<Immediate>(tokens[1])->immediate)
                );
        } else if (tokens[1]->type() == Token::LABEL && !tokens[1]->is_error) {
                if (!first_time) {
                        auto label = std::find_if(assembler.symbols.begin(), assembler.symbols.end(),
                                                  [this, tokens](std::pair<std::uint16_t, std::shared_ptr<Label>> symbol) -> bool {
                                                          return symbol.second->word == tokens[1]->word;
                                                  }
                        );

                        if (label == assembler.symbols.end()) {
                                tokens[1]->expected("valid label");
                        } else {
                                assembled.push_back(label->second->address);
                        }
                }
        }

        first_time = false;
        return 1;
}

Token::token_type Fill::type() const
{
        return Token::token_type::DIR_FILL;
}
