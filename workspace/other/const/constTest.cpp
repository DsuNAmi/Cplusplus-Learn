#include <iostream>


/* 
test const point
*/
void TestOne()
{
    int val = 3;
    int const * ptr = &val;
    std::cout << *ptr << std::endl;
    val = 4;
    std::cout << *ptr << std::endl;
}

int main()
{
    TestOne();
}