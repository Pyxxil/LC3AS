#ifndef LC3_SIMULATOR_TOKEN_HPP
#define LC3_SIMULATOR_TOKEN_HPP

#include <string>
#include <vector>
#include <memory>
#include <iostream>

class Assembler;

class Token
{
public:
        Token() = default;
        Token(std::string &word, int line = 0)
                : at_line(line), word(word)
        {}

        virtual ~Token() = default;

        bool is_error = false;
        int  at_line  = 0;

        std::string word;

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
                        std::cerr << "Line " << std::dec << at_line << ": ";
                }
                std::cerr << "Expected " << expects << ". Found " << word << "' instead\n";

                is_error = true;

                return *this;
        }

        virtual Token &note(std::string)
        { return *this; }

        virtual std::int32_t assemble(std::vector<std::shared_ptr<Token>> &tokens, Assembler &assembler)
        {
                std::cerr << word << ".assemble() not implemented\n";
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
                std::cerr << "ERROR: ";
                if (at_line) {
                        std::cerr << "Line " << std::dec << at_line << ": ";
                }
                std::cerr << word << "expects " << expected << " argument" << (expected == 1 ? "" : "'s")
                          << ", but %ld argument%s provided.\n";

                is_error = true;
        }
};

#endif // LC3_SIMULATOR_TOKEN_HPP
