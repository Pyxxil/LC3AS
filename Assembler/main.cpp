#include "Library/includes/Assembler.hpp"

int main(int argc, char **argv)
{
        if (argc != 2) {
                return 0;
        }

        std::string fileName = argv[1];

        Assembler as;
        as.tokenizeFile(fileName);

        return 0;
}