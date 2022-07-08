#include <iostream>

struct Base
{
    int v1;
    private:
    int v3;
    public:
    int v2;
    void print()
    {
        printf("%s\n","Hello World");
    }
};

int main()
{
    struct Base b1;
    Base b2;
    Base base;
    base.v1 = 1;
    base.v2 = 2;
    base.print();

    return 0;
}