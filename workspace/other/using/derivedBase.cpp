#include <iostream>

class Base
{
    public:
        std::size_t size() const {return n;}
    protected:
        std::size_t n;
};

class Derived : private Base
{
    public:
        using Base::size;
    protected:
        using Base::n;
};

int main()
{
    Derived d;
    std::cout << d.size() << std::endl;
    return 0;
}