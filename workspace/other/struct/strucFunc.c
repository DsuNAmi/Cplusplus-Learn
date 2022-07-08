#include <stdio.h>

struct Base
{   
    int v1;
    int v2;
    int v3;
    // void print();//
};

void Base()
{
    printf("%s","I am Base func");
}

int main()
{
    struct Base base;
    base.v1 = 1;
    printf("%d",base.v1);
    Base();
    return 0;
}
