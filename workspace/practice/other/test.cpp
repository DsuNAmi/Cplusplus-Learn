#include <iostream>


int a = 0;
int * outP = &a;
int * inP = nullptr;

void testFunction()
{
    int a = 1;
    inP = &a;
    printf("inP is : %x\n", inP);
    printf("inP value is : %d\n", *inP);

    return;
}

int main()
{
    printf("outP is : %x\n", outP);
    printf("outP value is : %d\n", *outP);

    testFunction();

    printf("inP is : %x\n",inP);
    printf("inP value is : %d\n", *inP);

    return 0;
}