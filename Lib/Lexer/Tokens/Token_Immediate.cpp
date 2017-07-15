#include <Tokens/Token_Immediate.hpp>

#include "Diagnostics.hpp"
#include "LexHelper.hpp"

Immediate::Immediate()
        : Token()
{

}

Immediate::Immediate(std::string &immediate,
                     std::string &immediate_uppercase,
                     std::string &t_file,
                     size_t line_number,
                     size_t column)
        : Token(immediate, immediate_uppercase, t_file, line_number, column)
{

}

void Immediate::requires_too_many_bits(int allowed_bits,
                                       bool is_signed,
                                       const Token *const caller,
                                       const std::map<std::string, Symbol> &symbols)
{
        (void) symbols;
        (void) caller;

        Diagnostics::Diagnostic diag(
                Diagnostics::FileContext(
                        Diagnostics::Variant<std::string>(file, Console::FOREGROUND_COLOUR::YELLOW),
                        Diagnostics::Variant<size_t>(at_line, Console::FOREGROUND_COLOUR::YELLOW),
                        Diagnostics::Variant<size_t>(at_column, Console::FOREGROUND_COLOUR::YELLOW)
                ), "Address too far away.", Diagnostics::INVALID_LABEL, Diagnostics::ERROR
        );

        std::stringstream ss;
        ss << "Immediate value '" << token << "' can not be represented as a "
           << allowed_bits << " bit " << (is_signed ? "signed" : "unsigned") << " value.";

        diag.provide_context(
                std::make_unique<Diagnostics::HighlightContext>(
                        Diagnostics::SelectionContext(
                                Diagnostics::FileContext(
                                        Diagnostics::Variant<std::string>(file, Console::FOREGROUND_COLOUR::YELLOW),
                                        Diagnostics::Variant<size_t>(at_line, Console::FOREGROUND_COLOUR::YELLOW),
                                        Diagnostics::Variant<size_t>(at_column, Console::FOREGROUND_COLOUR::YELLOW)
                                ), '^', ss.str(), std::string(lexed_lines[file].at(at_line - 1))
                        ), '~', token.length()
                )
        );

        Diagnostics::push(diag);

        std::cerr << "Immediate value " << value << " can not be represented as a " << allowed_bits
                  << " bit " << (is_signed ? "signed" : "unsigned") << " PC offset.\n";
}

Token::token_type Immediate::type() const
{
        return Token::token_type::IMMEDIATE;
}
