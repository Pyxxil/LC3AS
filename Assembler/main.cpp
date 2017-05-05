#include "Library/includes/Assembler.hpp"

int main(int argc, char **argv)
{
        Assembler as(argc, argv);

        as.assemble();

        return 0;
}