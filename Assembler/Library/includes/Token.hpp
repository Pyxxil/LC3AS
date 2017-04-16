#ifndef LC3_SIMULATOR_TOKEN_HPP
#define LC3_SIMULATOR_TOKEN_HPP

#include <string>
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
        fprintf(stderr, str, args);
        fprintf(stderr, ".\n");
        va_end(args);
}

class Token
{
public:
        Token() = default;
        Token(std::string &word, int line = 0)
                : word(word), at_line(line)
        {}

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
                fprintf(stderr, "ERROR: ");
                if (at_line) {
                        fprintf(stderr, "Line %d: ", at_line);
                }
                fprintf(stderr, "Expected %s. Found '%s' instead\n", expects, word.c_str());

                is_error = true;

                return *this;
        }

        virtual Token &note(std::string)
        { return *this; }

        virtual std::int32_t assemble(std::vector<std::shared_ptr<Token>> &tokens, Assembler &assembler)
        {
                fprintf(stderr, "%s not implemented\n", word.c_str());
                (void) tokens;
                (void) assembler;
                return -1;
        }

        virtual std::vector<std::uint16_t> as_assembled()
        {
                return assembled;
        }

        virtual void invalid_argument_count(std::size_t provided, std::size_t expected)
        {
                provided -= 1;  // This is not the best idea, but because tokens.size() returns
                                // the number of arguments + the token itself, it's a little easier to do here.
                fprintf(stderr, "ERROR: ");
                if (at_line) {
                        fprintf(stderr, "Line %d: ", at_line);
                }
                fprintf(stderr, "%s expects %ld argument%s, but %ld argument%s provided.\n",
                        word.c_str(), expected, expected == 1 ? "" : "'s",
                        provided, provided == 1 ? "" : "'s");

                is_error = true;
        }
};

#endif // LC3_SIMULATOR_TOKEN_HPP
