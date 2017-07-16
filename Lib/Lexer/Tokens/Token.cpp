#include "Tokens/Token.hpp"

#include <map>
#include <Lib/Includes/LexHelper.hpp>

#include "Diagnostics.hpp"

Token::Token(std::string &t_token, std::string &t_token_uppercase, std::string &t_file, size_t line, size_t column)
        : token(t_token), token_uppercase(t_token_uppercase), file(t_file)
          ,at_line(line), at_column(column)
{

}

Token::token_type Token::type() const
{
        return Token::NONE;
}

std::string Token::deduce_type() const
{
        switch (type()) {
        case OP_BR:
        case OP_ST:
        case OP_LD:
        case OP_LDR:
        case OP_LEA:
        case OP_LDI:
        case OP_NOT:
        case OP_RTI:
        case OP_STI:
        case OP_STR:
        case OP_ADD:
        case OP_AND:
        case OP_JSR:
        case OP_RET:
        case OP_JMP:
        case OP_JSRR:
        case OP_TRAP:
                return std::string("Instruction");
        case DIR_END:
        case DIR_FILL:
        case DIR_BLKW:
        case DIR_ORIG:
        case DIR_STRINGZ:
#ifdef INCLUDE_ADDONS
        case ADDON_NEG:
        case ADDON_SUB:
        case ADDON_SET:
        case ADDON_LSHIFT:
#endif
                return std::string("Directive");
        case TRAP_IN:
        case TRAP_OUT:
        case TRAP_PUTS:
        case TRAP_GETC:
        case TRAP_HALT:
        case TRAP_PUTSP:
                return std::string("Trap Routine");
        case REGISTER:
                return std::string("Register");
        case LABEL:
                return std::string("Label");
        case IMMEDIATE:
                return std::string("Immediate Value");
        case _STRING:
                return std::string("String Literal");
        default:
                return std::string("None Type");
        }
}

void Token::expected(const char *const expects) const
{
        Diagnostics::Diagnostic diag(
                Diagnostics::FileContext(
                        Diagnostics::Variant<std::string>(file, Console::Colour(Console::FOREGROUND_COLOUR::YELLOW)),
                        Diagnostics::Variant<size_t>(at_line, Console::Colour(Console::FOREGROUND_COLOUR::YELLOW)),
                        Diagnostics::Variant<size_t>(at_column, Console::Colour(Console::FOREGROUND_COLOUR::YELLOW))
                ), "Expected " + std::string(expects), Diagnostics::SYNTAX, Diagnostics::ERROR
        );

        diag.provide_context(
                std::make_unique<Diagnostics::HighlightContext>(
                        Diagnostics::SelectionContext(
                                Diagnostics::FileContext(
                                        Diagnostics::Variant<std::string>(file, Console::Colour(Console::FOREGROUND_COLOUR::YELLOW)),
                                        Diagnostics::Variant<size_t>(at_line, Console::Colour(Console::FOREGROUND_COLOUR::YELLOW)),
                                        Diagnostics::Variant<size_t>(at_column, Console::Colour(Console::FOREGROUND_COLOUR::YELLOW))
                                ), '^', "Found '" + token + "' ( Type: " + deduce_type() + " ) instead",
                                std::string(lexed_lines[file].at(at_line - 1))
                        ), '~', token.length()
                )
        );

        Diagnostics::push(diag);
}

std::int32_t Token::assemble(std::vector<std::shared_ptr<Token>> &tokens,
                             const std::map<std::string, Symbol> &symbols,
                             uint16_t program_counter)
{
        (void) tokens;
        (void) symbols;
        (void) program_counter;
        expected("one of: Instruction, Label, or Directive");
        return -1;
}

const std::vector<uint16_t> Token::as_assembled() const
{
        return assembled;
}

void Token::invalid_argument_count(size_t provided, size_t expected) const
{
        provided -= 1;  // This is not the best idea, but because tokens.size() returns
        // the number of arguments + the token itself, it's a little easier to do here.
        std::cerr << "ERROR: In " << file.substr(file.find_last_of('/') + 1) << ' ';

        if (0u != at_line) {
                std::cerr << "at line " << std::dec << at_line << ": ";
        }

        std::cerr << token << " expects " << expected << " argument" << (expected == 1 ? "" : "'s") << ", but "
                  << (provided < expected ? "only " : "") << provided << " argument"
                  << (provided == 1 ? " was" : "'s were") << " provided.\n";
}

bool Token::valid_arguments(std::vector<std::shared_ptr<Token>> &tokens)
{
        (void) tokens;
        expected("one of: Instruction, Label, or Directive");
        return false;
}

std::int32_t Token::guess_memory_size(std::vector<std::shared_ptr<Token>> &tokens) const
{
        std::cerr << tokens.front()->token << ".guess_memory_size() not implemented\n";
        return -1;
}

std::string Token::disassemble(uint16_t &program_counter,
                               const std::string &symbol,
                               int width) const
{
        (void) program_counter;
        (void) symbol;
        (void) width;
        return token + " has no disassemble (" + deduce_type() + ")\n";
}
