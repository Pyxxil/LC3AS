#ifndef PROJECT_OCTAL_HPP
#define PROJECT_OCTAL_HPP

#include <Tokens/Token_Immediate.hpp>

class Octal : public Immediate
{
public:
        Octal(std::string &immediate, int line_number = 0);
};

#endif // PROJECT_OCTAL_HPP
