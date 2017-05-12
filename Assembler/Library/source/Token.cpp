#include <map>
#include "Token.hpp"

Token::Token()
        : token(), token_uppercase(), file(), assembled()
{

}

Token::Token(std::string &t_token, std::string &t_token_uppercase, std::string &t_file, int line)
        : token(t_token), token_uppercase(t_token_uppercase), file(t_file)
          , assembled(), at_line(line), is_valid(true)
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
                return std::string("NoneType");
        }
}

void Token::expected(const char *const expects) const
{
        std::cerr << "ERROR: In " << file.substr(file.find_last_of('/') + 1) << ' ';
        if (at_line) {
                std::cerr << "at line " << std::dec << at_line << ": ";
        }
        std::cerr << "Expected " << expects << ". Found '" << token << "' ( Type: " << deduce_type() << " ) instead.\n";
}

std::int32_t Token::assemble(std::vector<std::shared_ptr<Token>> &tokens,
                             const std::map<std::string, Symbol> &symbols,
                             std::uint16_t program_counter)
{
        (void) tokens;
        (void) symbols;
        (void) program_counter;
        expected("one of: instruction, label, or directive");
        return -1;
}

const std::vector<uint16_t> Token::as_assembled() const
{
        return assembled;
}

void Token::invalid_argument_count(std::size_t provided, std::size_t expected) const
{
        provided -= 1;  // This is not the best idea, but because tokens.size() returns
        // the number of arguments + the token itself, it's a little easier to do here.
        std::cerr << "ERROR: In " << file.substr(file.find_last_of('/') + 1) << ' ';

        if (at_line) {
                std::cerr << "at line " << std::dec << at_line << ": ";
        }

        std::cerr << token << " expects " << expected << " argument" << (expected == 1 ? "" : "'s") << ", but "
                  << (provided < expected ? "only " : "") << provided << " argument"
                  << (provided == 1 ? " was" : "'s were") << " provided.\n";
}

bool Token::valid_arguments(std::vector<std::shared_ptr<Token>> &tokens)
{
        (void) tokens;
        expected("one of: instruction, label, or directive");
        return false;
}

std::int32_t Token::guess_memory_size(std::vector<std::shared_ptr<Token>> &tokens) const
{
        std::cerr << tokens.front()->token << ".guess_memory_size() not implemented\n";
        return -1;
}

std::string Token::disassemble(std::uint16_t &program_counter,
                               const std::string &symbol,
                               int width) const
{
        (void) program_counter;
        (void) symbol;
        (void) width;
        return token + " has no disassemble (" + deduce_type() + ")\n";
}

void Token::requires_too_many_bits(int allowed_bits, bool is_signed)
{
        is_valid = false;
        (void) allowed_bits;
        (void) is_signed;

        std::cerr << "ERROR: In " << file.substr(file.find_last_of('/') + 1) << ' ';
        if (at_line) {
                std::cerr << "Line " << std::dec << at_line << ": ";
        }
}
