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
        Token(std::string &token, int line = 0)
                : word(token), assembled(), at_line(line), is_valid(true)
        {}

        virtual ~Token() = default;

        std::string word;

        std::vector<std::uint16_t> assembled;

        int  at_line  = 0;
        bool is_valid = true;

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
#ifdef INCLUDE_ADDONS
                ADDON_NEG,
                ADDON_SUB,
#endif
        };

        virtual Token::token_type type() const
        { return Token::NONE; }

        std::string deduce_type() const
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
                        return std::string("Directive");
#endif
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

        virtual void expected(const char *const expects) const
        {
                std::cerr << "ERROR: ";
                if (at_line) {
                        std::cerr << "Line " << std::dec << at_line << ": ";
                }
                std::cerr << "Expected " << expects << ". Found '" << word << "' ( Type: " << deduce_type() << " ) instead.\n";
        }

        virtual void note(std::string)
        {}

        virtual std::int32_t assemble(std::vector<std::shared_ptr<Token>> &tokens, Assembler &assembler)
        {
                std::cerr << word << ".assemble() not implemented\n";
                (void) tokens;
                (void) assembler;
                return -1;
        }

        virtual const std::vector<uint16_t> as_assembled() const
        {
                return assembled;
        }

        virtual void invalid_argument_count(std::size_t provided, std::size_t expected) const
        {
                provided -= 1;  // This is not the best idea, but because tokens.size() returns
                // the number of arguments + the token itself, it's a little easier to do here.
                std::cerr << "ERROR: ";
                if (at_line) {
                        std::cerr << "Line " << std::dec << at_line << ": ";
                }
                std::cerr << word << " expects " << expected << " argument" << (expected == 1 ? "" : "'s")
                          << ", but " << (provided < expected ? "only " : "c") << provided << " argument"
                          << (provided == 1 ? " was" : "'s were") << " provided.\n";
        }

        virtual bool valid_arguments(std::vector<std::shared_ptr<Token>> &tokens)
        {
                (void) tokens;
                expected("one of: instruction, label, or directive");
                return false;
        }

        virtual std::int32_t guess_memory_size(std::vector<std::shared_ptr<Token>> &tokens) const
        {
                std::cerr << tokens.front()->word << ".guess_memory_size() not implemented\n";

                (void) tokens;
                return 0;
        }
};

#endif // LC3_SIMULATOR_TOKEN_HPP
