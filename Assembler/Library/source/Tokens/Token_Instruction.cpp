#include "Tokens/Token_Instruction.hpp"

Instruction::Instruction()
        : Token()
{

}

Instruction::Instruction(std::string &instruction, std::string &instruction_uppercase, int line_number)
        : Token(instruction, instruction_uppercase, line_number)
{

}
