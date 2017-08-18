#ifndef TOKEN_INSTRUCTION_HPP
#define TOKEN_INSTRUCTION_HPP

#include "Token.hpp"

class Instruction : public Token
{
public:
  Instruction() = default;
  Instruction(const std::string& instruction,
              const std::string& instruction_uppercase,
              const std::string& t_file,
              size_t line_number,
              size_t t_column);
};

#endif // TOKEN_INSTRUCTION_HPP
