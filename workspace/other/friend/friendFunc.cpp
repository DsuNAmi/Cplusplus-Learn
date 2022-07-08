#include <iostream>

using namespace std;

class A
{
    public:
        A(int _a) : a(_a){};
        friend int getA(A & ca);//friend Function
    private:
        int a;
};


int getA(A & ca)
{
    return ca.a;
}

int main()
{
    A a(3);
    cout << getA(a) << endl;

    return 0;
}