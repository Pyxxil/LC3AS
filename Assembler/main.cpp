#include "Library/includes/Assembler.hpp"

#include <iostream>

int main(int argc, char **argv)
{
        if (argc != 2) {
                std::cout << "No input files" << std::endl;
                return 1;
        }

        Assembler as;

        as.assemble(argv[1]);

        return 0;
}