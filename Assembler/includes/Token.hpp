#ifndef LC3_SIMULATOR_TOKEN_HPP
#define LC3_SIMULATOR_TOKEN_HPP

#include <iostream>
#include <tic.h>

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

        Token operator=(Token &other)
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

        virtual token_type type()
        { return token_type::NONE; }

        virtual uint64_t requires()
        { return static_cast<uint64_t >(token_type::NONE); }

        virtual Token expected(std::string expects)
        {
                std::cerr << "ERROR: ";
                if (at_line) {
                        std::cerr << "Line " << at_line << ": ";
                }
                std::cerr << "Expected " << expects << ". Found '" << word << "' instead." << std::endl;

                is_error = true;

                return *this;
        }

        virtual Token note(std::string)
        { return *this; }

        virtual uint16_t assemble() {
                return 0;
        }
};

class String : public Token
{
public:
        String(std::string &string, int line_number = 0) : Token(string, line_number)
        {
                std::cout << '"' << string << '"';
        }

        Token::token_type type() override
        {
                return Token::token_type::_STRING;
        }

        Token expected(std::string expects) override
        {
                std::cerr << "ERROR: ";
                if (at_line) {
                        std::cerr << "Line " << at_line << ": ";
                }
                std::cerr << "Expected " << expects << "." << std::endl;

                is_error = true;

                return *this;
        }
};

class Immediate : public Token
{
public:
        Immediate() : Token()
        {}
        Immediate(std::string &word, int line_number = 0) : Token(word, line_number)
        {}

        uint16_t immediate = 0;

        Token::token_type type() override
        { return Token::token_type::IMMEDIATE; }
};

class Decimal : public Immediate
{
public:
        Decimal(std::string &value, int line_number = 0) : Immediate(value, line_number)
        {
                if (value.length() > 6) {
                        Token::expected("decimal value up to 16 bits in length");
                        return;
                }

                try {
                        immediate = static_cast<uint16_t>(std::strtol(value.c_str(), nullptr, 10));
                } catch (const std::invalid_argument &e) {
                        Token::expected("valid decimal value");
                }
        }

        uint64_t requires() override
        { return static_cast<uint64_t >(Token::token_type::NONE); }
};

class Hexadecimal : public Immediate
{
public:
        Hexadecimal(std::string &value, int line_number = 0) : Immediate(value, line_number)
        {
                if (value.length() > 4) {
                        Token::expected("hexadecimal value up to 16 bits in length");
                        return;
                }

                try {
                        immediate = static_cast<uint16_t>(std::strtoul(value.c_str(), nullptr, 16));
                } catch (const std::invalid_argument &e) {
                        Token::expected("valid hexadecimal value");
                }
        }

        uint64_t requires() override
        { return static_cast<uint64_t >(Token::token_type::NONE); }
};

class Binary : public Immediate
{
public:
        Binary(std::string &value, int line_number = 0) : Immediate(value, line_number)
        {
                if (value.length() > 16) {
                        Token::expected("binary value up to 16 bits");
                        return;
                }

                try {
                        immediate = static_cast<uint16_t >(std::bitset<16>(value).to_ulong());
                } catch (const std::invalid_argument &e) {
                        Token::expected("valid binary value");
                }
        }

        uint64_t requires() override
        { return static_cast<uint64_t >(Token::token_type::NONE); }
};

class Register : public Token
{
public:
        Register(std::string which, int line_number = 0) : Token(which, line_number), reg(which.at(1))
        {
                // Valid registers are R0, R1, R2, R3, R4, R5, R6, and R7
                if (reg > 0x37 || reg < 0x30) {
                        expected("register between 0 and 7");
                }
        }

        char reg;

        Token::token_type type() override
        { return Token::token_type::REGISTER; }

        uint64_t requires() override
        { return static_cast<uint64_t >(Token::token_type::NONE); }
};

class Label : public Token
{

        // TODO: Add some sort of fuzzy match to check if the user meant something else
public:
        Label(std::string &name, int line_number) : Token(name, line_number), label(name)
        {}

        std::string label;

        Token::token_type type() override
        { return Token::token_type::LABEL; }

        uint64_t requires() override
        { return static_cast<uint64_t >(Token::token_type::NONE); }
};

/*
 * Directives
 */

class Directive : public Token
{
public:
        Directive() : Token()
        {}
        Directive(std::string &word, int line_number = 0) : Token(word, line_number)
        {}
};

class Stringz : public Directive
{
public:
        Stringz(std::string &word, int line_number = 0) : Directive(word, line_number)
        {}

        Token::token_type type() override
        { return Token::token_type::DIR_STRINGZ; }

        uint64_t requires() override
        { return static_cast<uint64_t >(Token::token_type::_STRING); }
};

class Blkw : public Directive
{
public:
        Blkw(std::string &word, int line_number = 0) : Directive(word, line_number)
        {}

        Token::token_type type() override
        { return Token::token_type::DIR_BLKW; }

        uint64_t requires() override
        {
                return static_cast<uint64_t >(Token::token_type::IMMEDIATE);
        }
};

class Fill : public Directive
{
public:
        Fill(std::string &word, int line_number = 0) : Directive(word, line_number)
        {}

        Token::token_type type() override
        { return Token::token_type::DIR_FILL; }

        uint64_t requires() override
        {
                return static_cast<uint64_t >(Token::token_type::IMMEDIATE);
        }
};

class Orig : public Directive
{
public:
        Orig(std::string &word, int line_number = 0) : Directive(word, line_number)
        {}

        Token::token_type type() override
        { return Token::token_type::DIR_ORIG; }

        uint64_t requires() override
        {
                return static_cast<uint64_t >(Token::token_type::IMMEDIATE);
        }
};

class End : public Directive
{
public:
        End(std::string &word, int line_number = 0) : Directive(word, line_number)
        {}

        Token::token_type type() override
        { return Token::token_type::DIR_END; }

        uint64_t requires() override
        { return static_cast<uint64_t >(Token::token_type::NONE); }
};

/*
 * Operators
 */


class Operator : public Token
{
public:
        Operator() : Token()
        {}
        Operator(std::string &oper, int line_number = 0) : Token(oper, line_number)
        {}
};

class Br : public Operator
{
public:
        Br(std::string &oper, int line_number = 0, bool n = false, bool z = false, bool p = false)
                : Operator(oper, line_number), N(n), Z(z), P(p)
        {}

        Token::token_type type() override
        {
                return Token::token_type::OP_BR;
        }

        uint64_t requires() override
        {
                return static_cast<uint64_t>(Token::token_type::REGISTER | Token::token_type::IMMEDIATE);
        }

        uint16_t assemble() override
        {
                return static_cast<uint16_t >(0x0 | N << 10 | Z << 9 | P << 8);
        }

private:
        bool N;
        bool Z;
        bool P;
};

class And : public Operator
{
public:
        And(std::string &oper, int line_number = 0) : Operator(oper, line_number)
        {}

        Token::token_type type() override
        {
                return Token::token_type::OP_AND;
        }

        uint64_t requires() override
        {
                return static_cast<uint64_t>(Token::token_type::REGISTER | Token::token_type::IMMEDIATE);
        }
};

class Add : public Operator
{
public:
        Add(std::string &oper, int line_number = 0) : Operator(oper, line_number)
        {}

        Token::token_type type() override
        {
                return Token::token_type::OP_ADD;
        }

        uint64_t requires() override
        {
                return static_cast<uint64_t>(Token::token_type::REGISTER | Token::token_type::IMMEDIATE);
        }
};

class Not : public Operator
{
public:
        Not(std::string &oper, int line_number = 0) : Operator(oper, line_number)
        {}

        Token::token_type type() override
        {
                return Token::token_type::OP_NOT;
        }

        uint64_t requires() override
        {
                return static_cast<uint64_t>(Token::token_type::REGISTER | Token::token_type::IMMEDIATE);
        }
};

class Jsr : public Operator
{
public:
        Jsr(std::string &oper, int line_number = 0) : Operator(oper, line_number)
        {}

        Token::token_type type() override
        {
                return Token::token_type::OP_JSR;
        }

        uint64_t requires() override
        {
                return static_cast<uint64_t>(Token::token_type::LABEL);
        }
};

class Jsrr : public Operator
{
public:
        Jsrr(std::string &oper, int line_number = 0) : Operator(oper, line_number)
        {}

        Token::token_type type() override
        {
                return Token::token_type::OP_JSRR;
        }

        uint64_t requires() override
        {
                return static_cast<uint64_t>(Token::token_type::REGISTER);
        }
};

class Jmp : public Operator
{
public:
        Jmp(std::string &oper, int line_number = 0) : Operator(oper, line_number)
        {}

        Token::token_type type() override
        {
                return Token::token_type::OP_JMP;
        }

        uint64_t requires() override
        {
                return static_cast<uint64_t>(Token::token_type::REGISTER);
        }
};

class St : public Operator
{
public:
        St(std::string &oper, int line_number = 0) : Operator(oper, line_number)
        {}

        Token::token_type type() override
        {
                return Token::token_type::OP_ST;
        }

        uint64_t requires() override
        {
                return static_cast<uint64_t>(Token::token_type::REGISTER | Token::token_type::IMMEDIATE);
        }
};

class Str : public Operator
{
public:
        Str(std::string &oper, int line_number = 0) : Operator(oper, line_number)
        {}

        Token::token_type type() override
        {
                return Token::token_type::OP_STR;
        }

        uint64_t requires() override
        {
                return static_cast<uint64_t>(Token::token_type::REGISTER | Token::token_type::IMMEDIATE);
        }
};

class Sti : public Operator
{
public:
        Sti(std::string &oper, int line_number = 0) : Operator(oper, line_number)
        {}

        Token::token_type type() override
        {
                return Token::token_type::OP_STI;
        }

        uint64_t requires() override
        {
                return static_cast<uint64_t>(Token::token_type::REGISTER | Token::token_type::IMMEDIATE);
        }
};

class Ld : public Operator
{
public:
        Ld(std::string &oper, int line_number = 0) : Operator(oper, line_number)
        {}

        Token::token_type type() override
        {
                return Token::token_type::OP_LD;
        }

        uint64_t requires() override
        {
                return static_cast<uint64_t>(Token::token_type::REGISTER | Token::token_type::IMMEDIATE);
        }
};

class Lea : public Operator
{
public:
        Lea(std::string &oper, int line_number = 0) : Operator(oper, line_number)
        {}

        Token::token_type type() override
        {
                return Token::token_type::OP_LEA;
        }

        uint64_t requires() override
        {
                return static_cast<uint64_t>(Token::token_type::REGISTER | Token::token_type::IMMEDIATE);
        }
};

class Ldi : public Operator
{
public:
        Ldi(std::string &oper, int line_number = 0) : Operator(oper, line_number)
        {}

        Token::token_type type() override
        {
                return Token::token_type::OP_LDI;
        }

        uint64_t requires() override
        {
                return static_cast<uint64_t>(Token::token_type::REGISTER | Token::token_type::IMMEDIATE);
        }
};

class Ldr : public Operator
{
public:
        Ldr(std::string &oper, int line_number = 0) : Operator(oper, line_number)
        {}

        Token::token_type type() override
        {
                return Token::token_type::OP_LDR;
        }

        uint64_t requires() override
        {
                return static_cast<uint64_t>(Token::token_type::REGISTER | Token::token_type::IMMEDIATE);
        }
};

class Trap : public Operator
{
public:
        Trap(std::string &word, int line_number = 0) : Operator(word, line_number)
        {}

        Token::token_type type() override
        {
                return Token::token_type::OP_TRAP;
        }

        uint64_t requires() override
        {
                return static_cast<uint64_t >(Token::token_type::IMMEDIATE);
        }
};

class Puts : public Operator
{
public:
        Puts(std::string &oper, int line_number = 0) : Operator(oper, line_number)
        {}

        Token::token_type type() override
        {
                return Token::token_type::TRAP_PUTS;
        }

        uint64_t requires() override
        {
                return static_cast<uint64_t>(Token::token_type::NONE);
        }
};

class Halt : public Operator
{
public:
        Halt(std::string &oper, int line_number = 0) : Operator(oper, line_number)
        {}

        Token::token_type type() override
        {
                return Token::token_type::TRAP_HALT;
        }

        uint64_t requires() override
        {
                return static_cast<uint64_t>(Token::token_type::NONE);
        }
};

class Putsp : public Operator
{
public:
        Putsp(std::string &oper, int line_number = 0) : Operator(oper, line_number)
        {}

        Token::token_type type() override
        {
                return Token::token_type::TRAP_PUTSP;
        }

        uint64_t requires() override
        {
                return static_cast<uint64_t>(Token::token_type::NONE);
        }
};

class Getc : public Operator
{
public:
        Getc(std::string &oper, int line_number = 0) : Operator(oper, line_number)
        {}

        Token::token_type type() override
        {
                return Token::token_type::TRAP_GETC;
        }

        uint64_t requires() override
        {
                return static_cast<uint64_t>(Token::token_type::NONE);
        }
};

class Out : public Operator
{
public:
        Out(std::string &oper, int line_number = 0) : Operator(oper, line_number)
        {}

        Token::token_type type() override
        {
                return Token::token_type::TRAP_OUT;
        }

        uint64_t requires() override
        {
                return static_cast<uint64_t>(Token::token_type::NONE);
        }
};

class In : public Operator
{
public:
        In(std::string &oper, int line_number = 0) : Operator(oper, line_number)
        {}

        Token::token_type type() override
        {
                return Token::token_type::TRAP_IN;
        }

        uint64_t requires() override
        {
                return static_cast<uint64_t>(Token::token_type::NONE);
        }
};

#endif // LC3_SIMULATOR_TOKEN_HPP
