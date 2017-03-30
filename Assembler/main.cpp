#include <iostream>

#include "includes/Assembler.hpp"

class Base
{
public:
        Base() { }

        virtual void sayHello() { std::cout << "Base"; }

        virtual ~Base() = default;
};

class Sub : public Base
{
public:
        Sub() : Base() { }

        void sayHello() { std::cout << "Sub"; };
};

int main(int, char **argv)
{
        std::string fileName = argv[1];
        Assembler::parseFile(fileName);

        Base b;
        Sub s;

        b.sayHello();
        s.sayHello();

        return 0;
}