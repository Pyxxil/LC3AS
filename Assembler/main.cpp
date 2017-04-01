#include <iostream>

#include "includes/Assembler.hpp"

int main(int, char **argv)
{
        std::string fileName = argv[1];

        std::string line = "ADD R0, R0, R1";

        Assembler as;
        as.tokenizeFile(fileName);

        as.tokenizeLine(line);

        return 0;
}