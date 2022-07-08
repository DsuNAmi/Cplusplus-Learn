/**
 * @file rule4&5.cpp
 * @author your name (you@domain.com)
 * @brief 
 * 1.普通单类继承，继承就是基类+派生类自身的大小。（注意字节对齐）
 * PS: 类的数据成员函数按其声明顺序加入内存，与访问权限无关，只看声明顺序
 * 2.虚单继承，派生类继承基类vptr
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
        char a;
        int b;
};

/**
 * @brief 此时按照顺序：
 * char a
 * int b 
 * short a
 * long b
 * 根据字节对齐 4 + 4 + 8 + 8 = 24
 * 
 * 
 * 或者编译器优化
 * char a 
 * short a
 * int b
 * long b
 * 根据字节对齐 2 + 2 + 4 + 8 = 16
 */

class B:A
{
    public:
        short a;
        long b;
};
/**
 * 把A成员拆开看，char为1，int 为4 , 1 + (3) + 4 + 1 + (3) = 12， （字节对齐）
 * 
 */

class C
{
    A a;
    char c;
};

class A1
{
    virtual void fun();
};

class C1:public A1
{

};

int main()
{
    cout << sizeof(A) << endl; //8
    cout << sizeof(B) << endl; //16
    cout << sizeof(C) << endl; //12
    cout << sizeof(C1) << endl; //8
    return 0;
}
