#include "Tokens/Token_Instruction.hpp"

Instruction::Instruction(std::string& instruction,
                         std::string& instruction_uppercase,
                         std::string& t_file,
                         size_t line_number,
                         size_t column)
  : Token(instruction, instruction_uppercase, t_file, line_number, column)
{}
