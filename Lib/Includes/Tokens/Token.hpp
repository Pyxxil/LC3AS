#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <array>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "Symbol.hpp"

enum Signedness : bool {
  UNSIGNED = false,
  SIGNED = true,
};

enum Expected {
  REGISTER = 0,
  IMMEDIATE_VALUE = 1,
  VALID_TRAP_VEC = 2,
  REGISTER_OR_IMMEDIATE_VAL = 3,
  LABEL_OR_IMMEDIATE_VAL = 4,
  STRING_LITERAL = 5,
  ORIG_STATEMENT = 6,
  INSTRUCTION_LABEL_OR_DIRECTIVE = 7,
};

static const std::array<std::string, 8> expects{
    "Expected register",
    "Expected immediate value",
    "Expected valid trap vector",
    "Expected register or immediate value",
    "Expected label or immediate value",
    "Expected string literal",
    "Expected .ORIG statement",
    "Expected one of: Instruction, Label, or Directive",
};

class Token {
public:
  Token() = default;
  Token(const std::string &t_token, const std::string &t_token_uppercase,
        const std::string &t_file, size_t t_line, size_t t_column)
      : line(t_line), column(t_column), file(t_file), token(t_token),
        token_uppercase(t_token_uppercase) {}
  Token(const Token &other) = default;
  Token(Token &&other) noexcept = default;

  Token &operator=(const Token &rhs) = default;
  Token &operator=(Token &&rhs) noexcept = default;

  virtual ~Token() = default;

  enum token_type {
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
    ADDON_SET,
    ADDON_LSHIFT,
    ADDON_INCLUDE,
    ADDON_JMPT,
#endif
  };

  inline virtual token_type type() const { return NONE; }

  std::string deduce_type() const;

  virtual int32_t assemble(std::vector<std::shared_ptr<Token>> &tokens,
                           const std::map<std::string, Symbol> &symbols,
                           uint16_t program_counter);

  virtual void expected(Expected expect) const;
  virtual void invalid_argument_count(size_t provided, size_t expected,
                                      size_t last_column) const;

  virtual bool valid_arguments(std::vector<std::shared_ptr<Token>> &tokens) {
    (void)tokens;
    expected(Expected::INSTRUCTION_LABEL_OR_DIRECTIVE);
    return false;
  }

  virtual uint16_t
  guess_memory_size(std::vector<std::shared_ptr<Token>> &tokens) const {
    (void)tokens;
    expected(Expected::INSTRUCTION_LABEL_OR_DIRECTIVE);
    return 0;
  }

  virtual std::string disassemble(uint16_t &program_counter,
                                  const std::string &symbol, int width) const {
    (void)program_counter;
    (void)symbol;
    (void)width;
    expected(Expected::INSTRUCTION_LABEL_OR_DIRECTIVE);
    return std::string();
  }

  virtual void
  requires_too_many_bits(int allowed_bits, Signedness is_signed,
                         const Token *const caller,
                         const std::map<std::string, Symbol> &symbols) {
    (void)allowed_bits;
    (void)is_signed;
    (void)caller;
    (void)symbols;
  }

  inline const std::vector<uint16_t> &as_assembled() const { return assembled; }

  size_t line{};
  size_t column{};

  std::string file;
  std::string token;
  std::string token_uppercase;

  bool is_valid = true;

  std::vector<uint16_t> assembled{};
};

#endif // TOKEN_HPP
