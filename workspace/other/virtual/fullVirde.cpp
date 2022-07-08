#include <iostream>

using namespace std;


class Base
{
    public:
        Base() {cout << "Constructor Base\n";}
        virtual ~Base() {cout << "Destructor Base\n";} //不写虚函数，子类的析构函数不会调用
};


class Derived : public Base
{
    public:
        Derived() {cout << "Constructor Derived\n";}
        ~Derived() {cout << "Destructor Derived\n";}
};


int main()
{
    Derived * d = new Derived();
    Base * b = d;
    delete b;
    return 0;
}