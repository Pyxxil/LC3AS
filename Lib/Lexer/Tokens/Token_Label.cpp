#include "Tokens/Token_Label.hpp"

#include "Diagnostics.hpp"
#include "LexHelper.hpp"
#include "String_Matcher.hpp"

Label::Label(std::string &name, std::string &t_file, size_t line_number,
             size_t column)
    : Token(name, name, t_file, line_number, column)
{
}

int32_t Label::assemble(std::vector<std::shared_ptr<Token>> &tokens,
                        const std::map<std::string, Symbol> &symbols,
                        uint16_t program_counter)
{
    if (!is_valid) {
        return -1;
    }

    if (tokens.size() == 1) {
        return 0;
    }

    std::vector<std::shared_ptr<Token>> vec(tokens.begin() + 1, tokens.end());

    const int32_t ret = vec.front()->assemble(vec, symbols, program_counter);

    assembled = vec.front()->assembled;
    return ret;
}

bool Label::valid_arguments(std::vector<std::shared_ptr<Token>> &tokens)
{
    if (tokens.size() > 1) {
        instruction = tokens.at(1);
        std::vector<std::shared_ptr<Token>> vec(tokens.begin() + 1,
                                                tokens.end());
        return instruction->valid_arguments(vec);
    }

    return true;
}

uint16_t
Label::guess_memory_size(std::vector<std::shared_ptr<Token>> &tokens) const
{
    if (tokens.size() > 1) {
        std::vector<std::shared_ptr<Token>> vec(tokens.begin() + 1,
                                                tokens.end());
        return vec.front()->guess_memory_size(vec);
    }

    return 0;
}

void Label::not_found(const std::map<std::string, Symbol> &match_candidates)
{
    String_Matcher matcher(token);

    for (const auto &symbol : match_candidates) {
        matcher.consider(symbol.first);
    }

    auto &&possible_match = matcher.best_match();

    Diagnostics::Diagnostic diagnostic(
        Diagnostics::FileContext(file, line, column), "Invalid label",
        Diagnostics::INVALID_LABEL, Diagnostics::ERROR);

    if (!possible_match.empty()) {
        auto &&sym = match_candidates.find(possible_match);
        diagnostic.provide_context(
            std::make_unique<Diagnostics::HighlightContext>(
                Diagnostics::SelectionContext(
                    Diagnostics::FileContext(file, line, column), '^',
                    "No such label '" + token + "'; Did you mean '"
                        + possible_match + "'?",
                    lexed_lines[file].at(line)),
                '~', token.size(), possible_match));

        // TODO: Fix match candidates to provide file, and column.
        diagnostic.provide_context(
            std::make_unique<Diagnostics::HighlightContext>(
                Diagnostics::SelectionContext(
                    Diagnostics::FileContext(sym->second.file,
                                             sym->second.line_number,
                                             sym->second.column),
                    '^', "'" + possible_match + "' declared here",
                    lexed_lines[sym->second.file].at(sym->second.line_number)),
                '~', sym->first.length()));
    }

    Diagnostics::push(diagnostic);
}

std::string Label::disassemble(uint16_t &program_counter,
                               const std::string &symbol, int width) const
{
    if (!assembled.empty()) {
        return instruction->disassemble(program_counter, symbol, width);
    }

    return "";
}

void Label::requires_too_many_bits(int allowed_bits, bool is_signed,
                                   const Token *const caller,
                                   const std::map<std::string, Symbol> &symbols)
{
    (void)caller;

    Diagnostics::Diagnostic diagnostic(
        Diagnostics::FileContext(file, line, column),
        "Address too far away.", Diagnostics::INVALID_LABEL,
        Diagnostics::ERROR);

    std::stringstream error_string;
    error_string << "Address of '" << token
                 << "' requires offset which can't be represented in a "
                 << allowed_bits << " bit "
                 << (is_signed ? "signed" : "unsigned") << " PC offset";

    diagnostic.provide_context(std::make_unique<Diagnostics::HighlightContext>(
        Diagnostics::SelectionContext(
            Diagnostics::FileContext(file, line, column), '^',
            error_string.str(), lexed_lines[file].at(line)),
        '~', token.length()));

    const auto sym = symbols.find(token);
    diagnostic.provide_context(std::make_unique<Diagnostics::HighlightContext>(
        Diagnostics::SelectionContext(
            Diagnostics::FileContext(sym->second.file, sym->second.line_number,
                                     sym->second.column),
            '^', "'" + sym->first + "' declared here",
            lexed_lines[sym->second.file].at(sym->second.line_number)),
        '~', sym->first.length()));

    Diagnostics::push(diagnostic);
}

Token::token_type Label::type() const { return Token::token_type::LABEL; }
