#include <iostream>



class A
{
    private:
        A * next;
        static A * mFreePosi;           //总是指向一块可以分配出去的内存的首地址
        static int mSTrunkCount;        //一次分配多少倍该类的内存

    public:
        static void * operator new(std::size_t size);
        static void operator delete(void * phead);
        static int mICount;             //用于分配计数统计，每new 一次 + 1
        static int mIMallocCount;       //用于统计malloc次数，每malloc一次 + 1
};


void * A::operator new(std::size_t size)
{
    //内存池实现
    A * tempLink;
    if(mFreePosi == nullptr)
    {
        //如果为空，说明要申请内存，申请一块很大的内存
        std::size_t realSize = mSTrunkCount * size;     //申请内存
        mFreePosi = reinterpret_cast<A*>(new char[realSize]);       //传统new，调用底层malloc

        tempLink = mFreePosi;


        //把分配出来的这一块内存链接起来
        for(;tempLink != &mFreePosi[mSTrunkCount - 1]; ++tempLink)
        {
            tempLink->next = tempLink + 1;
        }
        tempLink->next = nullptr;
        ++mIMallocCount;    
    }
    tempLink = mFreePosi;
    mFreePosi = mFreePosi->next;
    ++mICount;
    return tempLink;
}


void A::operator delete(void * phead)
{
    (static_cast<A*>(phead))->next = mFreePosi;
    mFreePosi = static_cast<A*>(phead);
}


int A::mICount = 0;
int A::mIMallocCount = 0;

A * A::mFreePosi = nullptr;
int A::mSTrunkCount = 500;





int main()
{
    clock_t start, end;
    start = clock();
    for(int i = 0; i < 5000000; ++i)
    {
        A * pa = new A();
    }
    end = clock();
    std::cout << "申请分配的次数为：" << A::mICount << " 实际malloc次数为：" << A::mIMallocCount << "用时(毫秒)：" << end - start << std::endl;
    return 0;
}