#include "includes/Assembler.hpp"

int main(int, char **argv)
{
        std::string fileName = argv[1];

        Assembler as;
        as.tokenizeFile(fileName);

        return 0;
}