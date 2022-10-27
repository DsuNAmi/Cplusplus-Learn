#include <iostream>

class TestEP
{
    public:
        int m_i;
        int m_j;
    
    public:
        struct obj
        {
            struct obj * next;
        };
};



int main()
{
    TestEP mytest;
    std::cout << sizeof(mytest) << std::endl;
    TestEP::obj * ptmp;
    ptmp = (TestEP::obj * ) & mytest;       //把对象的首地址给了这个指针，这个指针指向对象mytest的首地址
    ptmp->next = nullptr;                   //前四个字节给成了00 00 00 00

    

    return 0;
}  