#include "Tokens/Token_Instruction.hpp"

Instruction::Instruction(const std::string& instruction,
                         const std::string& instruction_uppercase,
                         const std::string& t_file,
                         size_t line_number,
                         size_t t_column)
  : Token(instruction, instruction_uppercase, t_file, line_number, t_column)
{
}
