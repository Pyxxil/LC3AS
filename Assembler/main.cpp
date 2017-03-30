#include <iostream>

#include "includes/Assembler.hpp"

int main(int, char **argv)
{
        std::string fileName = argv[1];
        Assembler as;
        as.parseFile(fileName);

        return 0;
}