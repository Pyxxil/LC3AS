#include "Assembler.hpp"

::Assembler::Assembler *Assembler::assembler = nullptr;
std::string Assembler::last_error;

bool Assembler::init()
{
        assembler = new ::Assembler::Assembler();

        if (!assembler) {
                last_error = "Unable to allocate memory for the Assembler.";
                return false;
        }

        return true;
}

void Assembler::de_init()
{
        if (assembler) {
                delete assembler;
        }
}

Assembler::Assembler::Assembler()
{

}
