/**
 * @file static.cpp
 * @author DsuNAmi (you@domain.com)
 * @brief 静态数据成员
 * 静态数据成员被编译器放在程序的一个global data members中
 * 它是类的一个数据成员，但是不影响类的大小。
 * 不管这个类产生了多少实例，还是派生了多少类。
 * 静态数据成员只有一个实例。
 * 静态数据成员，一旦被声明，就已经存在。
 * @version 0.1
 * @date 2022-07-07
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <iostream>

using namespace std;

class A
{
    public:
        char b;
        virtual void func();
        static int c;
        static int d;
        static int f;
};


int main()
{
    /**
     * @brief 16 字节对齐，静态变量不影响类的大小，`vptr = 8bytes`
     * 
     */
    cout << sizeof(A) << endl;
    return 0;
}