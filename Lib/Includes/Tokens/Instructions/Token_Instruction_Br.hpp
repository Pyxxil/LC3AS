#ifndef TOKEN_OPERATOR_BR_HPP
#define TOKEN_OPERATOR_BR_HPP

#include "Tokens/Token_Instruction.hpp"

class Br : public Instruction
{
public:
  Br(std::string& instruction,
     std::string& instruction_uppercase,
     std::string& t_file,
     size_t line_number,
     size_t t_column,
     bool n,
     bool z,
     bool p);

  Br(std::string&& instruction,
     std::string&& instruction_uppercase,
     std::string& t_file,
     size_t line_number,
     size_t t_column,
     bool n,
     bool z,
     bool p);

  Token::token_type type() const override final;

  int32_t assemble(std::vector<std::shared_ptr<Token>>& tokens,
                   const std::map<std::string, Symbol>& symbols,
                   uint16_t program_counter) override final;
  uint16_t guess_memory_size(
    std::vector<std::shared_ptr<Token>>& tokens) const override final;

  bool valid_arguments(
    std::vector<std::shared_ptr<Token>>& tokens) override final;

  std::string disassemble(uint16_t& program_counter,
                          const std::string& symbol,
                          int width) const override final;

private:
  bool N;
  bool Z;
  bool P;

  std::shared_ptr<Token> provided;
};

#endif // TOKEN_OPERATOR_BR_HPP
