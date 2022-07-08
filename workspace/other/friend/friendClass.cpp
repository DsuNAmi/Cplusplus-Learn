#include <iostream>

using namespace std;

class A
{
    public:
        A(int _a) : a(_a) {}
        friend class B;
    private:
        int a;
};


class B
{
    public:
        int getB(A ca)
        {
            return ca.a;
        }
};


int main()
{
    A a(3);
    B b;
    cout << b.getB(a) << endl;
    return 0;
}