#include "Tokens/Token_Label.hpp"

#include "String_Matcher.hpp"

Label::Label(std::string &name, std::string &t_file, int line_number)
        : Token(name, name, t_file, line_number)
{

}

std::int32_t Label::assemble(std::vector<std::shared_ptr<Token>> &tokens,
                             const std::map<std::string, Symbol> &symbols,
                             std::uint16_t program_counter)
{
        if (!is_valid) {
                return -1;
        }

        if (tokens.size() == 1) {
                return 0;
        }

        std::vector<std::shared_ptr<Token>> vec(tokens.begin() + 1, tokens.end());

        std::int32_t ret = vec.front()->assemble(vec, symbols, program_counter);

        assembled = vec.front()->assembled;
        return ret;
}

bool Label::valid_arguments(std::vector<std::shared_ptr<Token>> &tokens)
{
        if (tokens.size() > 1) {
                instruction = tokens.at(1);
                std::vector<std::shared_ptr<Token>> vec(tokens.begin() + 1, tokens.end());
                return instruction->valid_arguments(vec);
        } else {
                return true;
        }
}

std::int32_t Label::guess_memory_size(std::vector<std::shared_ptr<Token>> &tokens) const
{
        if (tokens.size() > 1) {
                std::vector<std::shared_ptr<Token>> vec(tokens.begin() + 1, tokens.end());
                return vec.front()->guess_memory_size(vec);
        } else {
                return 0;
        }
}

void Label::not_found(const std::map<std::string, Symbol> &match_candidates)
{
        String_Matcher matcher(token);

        for (const auto &symbol : match_candidates) {
                matcher.consider(symbol.first);
        }

        const std::string possible_match = matcher.best_match();

        std::cerr << "ERROR: In " << file.substr(file.find_last_of('/') + 1) << ' ';
        if (at_line) {
                std::cerr << "at line " << std::dec << at_line << ": ";
        }
        std::cerr << "No such label '" << token << '\'';

        if (!possible_match.empty()) {
                std::cerr << "; Did you mean '" << possible_match << "'?\n";
        } else {
                std::cerr << ".\n";
        }
}

std::string Label::disassemble(std::uint16_t &program_counter,
                               const std::string &symbol,
                               int width) const
{
        if (!assembled.empty()) {
                return instruction->disassemble(program_counter, symbol, width);
        } else {
                return "";
        }
}

void Label::requires_too_many_bits(int allowed_bits, bool is_signed)
{
        Token::requires_too_many_bits(allowed_bits, false);

        std::cerr << "Address of '" << token << "' has an address not able to be represented in a "
                  << allowed_bits << " bit " << (is_signed ? "signed" : "unsigned") << " PC offset.\n";
}

Token::token_type Label::type() const
{
        return Token::token_type::LABEL;
}
