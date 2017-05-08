#include "Tokens/Token_Instruction.hpp"

Instruction::Instruction()
        : Token()
{

}

Instruction::Instruction(std::string &instruction,
                         std::string &instruction_uppercase,
                         std::string &t_file,
                         int line_number)
        : Token(instruction, instruction_uppercase, t_file, line_number)
{

}
