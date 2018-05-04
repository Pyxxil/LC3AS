#ifndef TOKEN_TRAP_GETC_HPP
#define TOKEN_TRAP_GETC_HPP

#include "Tokens/Token_Instruction.hpp"

class Getc : public Instruction {
public:
  Getc(const std::string &trap, const std::string &trap_uppercase,
       const std::string &t_file, size_t line_number, size_t t_column);

  inline token_type type() const final { return TRAP_GETC; }

  int32_t assemble(std::vector<std::shared_ptr<Token>> &tokens,
                   const std::map<std::string, Symbol> &symbols,
                   uint16_t program_counter) final;
  uint16_t
  guess_memory_size(std::vector<std::shared_ptr<Token>> &tokens) const final;

  bool valid_arguments(std::vector<std::shared_ptr<Token>> &tokens) final;

  std::string disassemble(uint16_t &program_counter, const std::string &symbol,
                          int width) const final;
};

#endif // TOKEN_TRAP_GETC_HPP
