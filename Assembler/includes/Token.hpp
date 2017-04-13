#ifndef LC3_SIMULATOR_TOKEN_HPP
#define LC3_SIMULATOR_TOKEN_HPP

#include <iostream>
#include <vector>
#include <memory>

class Assembler;

inline void ERROR(const char *const str, ...)
{
        va_list args;
        va_start(args, str);
        fprintf(stderr, "ERROR: ");
        fprintf(stderr, str, args);
        fprintf(stderr, ".\n");
        va_end(args);
}

inline void WARNING(const char *const str, ...)
{
        va_list args;
        va_start(args, str);
        fprintf(stderr, "WARNING: ");
        std::fprintf(stderr, str, args);
        fprintf(stderr, ".\n");
        va_end(args);
}

class Token
{
public:
        Token() = default;
        Token(std::string &word, int line = 0) : word(word), at_line(line)
        {}

        Token &operator=(Token &other)
        {
                word = other.word;
                is_error = other.is_error;
                at_line = other.at_line;

                return *this;
        }

        virtual ~Token() = default;

        std::string word;
        bool is_error = false;
        int at_line = 0;
        std::vector<std::uint16_t> assembled;

        enum token_type
        {
                NONE = 0,
                REGISTER,
                LABEL,
                IMMEDIATE,
                DIR_STRINGZ,
                DIR_FILL,
                DIR_BLKW,
                DIR_ORIG,
                DIR_END,
                OP_AND,
                OP_ADD,
                OP_BR,
                OP_JMP,
                OP_RET,
                OP_JSR,
                OP_JSRR,
                OP_RTI,
                OP_NOT,
                OP_LDI,
                OP_LEA,
                OP_LDR,
                OP_LD,
                OP_ST,
                OP_STI,
                OP_STR,
                OP_TRAP,
                TRAP_PUTS,
                TRAP_GETC,
                TRAP_IN,
                TRAP_OUT,
                TRAP_PUTSP,
                TRAP_HALT,
                _STRING,
        };

        virtual Token::token_type type() const
        { return Token::NONE; }

        virtual Token &expected(const char *const expects)
        {
                std::cerr << "ERROR: ";
                if (at_line) {
                        std::cerr << "Line " << at_line << ": ";
                }
                std::cerr << "Expected " << expects << ". Found '" << word << "' instead." << std::endl;

                is_error = true;

                return *this;
        }

        virtual Token &note(std::string)
        { return *this; }

        virtual int32_t assemble(std::vector<std::shared_ptr<Token>> &tokens, bool *orig_seen, bool *end_seen)
        {
                std::cerr << word << " not implemented\n";
                (void) tokens;
                (void) orig_seen;
                (void) end_seen;
                return -1;
        }

        virtual std::vector<std::uint16_t> as_assembled()
        {
                return assembled;
        }
};

#endif // LC3_SIMULATOR_TOKEN_HPP
