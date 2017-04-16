#include "Library/includes/Assembler.hpp"

#include <iostream>

int main(int argc, char **argv)
{
        if (argc != 2) {
                std::cout << "No input files" << std::endl;
                return 1;
        }

        std::string fileName = argv[1];
        std::string prefix = fileName.substr(0, fileName.find_first_of('.'));

        Assembler as;

        as.tokenizeFile(fileName);
        as.assemble();
        as.write(prefix);

        return 0;
}