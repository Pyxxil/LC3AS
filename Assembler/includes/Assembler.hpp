#ifndef LC3_SIMULATOR_ASSEMBLER_HPP
#define LC3_SIMULATOR_ASSEMBLER_HPP

#include <string>

class Assembler
{
public:
        Assembler();
        static void parseFile(std::string &fileName);
        static void parseLine(std::string &line); // This should return a vector

        static void decode(std::string &word);
};

#endif // LC3_SIMULATOR_ASSEMBLER_HPP
