#include "Tokens/Token_Instruction.hpp"

Instruction::Instruction() : Token()
{}

Instruction::Instruction(std::string &instruction, int line_number) : Token(instruction, line_number)
{}
