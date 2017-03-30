#ifndef LC3_SIMULATOR_ASSEMBLER_HPP
#define LC3_SIMULATOR_ASSEMBLER_HPP

#include <string>

#include "Token.hpp"

class Assembler
{
public:
        Assembler();
        void parseFile(std::string &fileName);
        void parseLine(std::string &line); // This should return a vector

        Token encode(std::string &word);
};

#endif // LC3_SIMULATOR_ASSEMBLER_HPP
