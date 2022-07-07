#include <iostream>

using namespace std;

class Base
{
    private:
        int x;
    public:
        virtual void show() = 0;
        int getX() {return x;}
};

class Derived : public Base
{
    public:
        void show() {cout << "In Derived \n";} //实现了抽象类的纯虚函数
        Derived() {} //构造函数

};


int main()
{
    // Base b; //error 
    Base * bp = new Derived(); //
    bp->show();
    return 0;
}