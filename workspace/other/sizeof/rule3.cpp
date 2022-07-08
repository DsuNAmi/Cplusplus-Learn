/**
 * @file rule3.cpp
 * @author your name (you@domain.com)
 * @brief 对于包含虚函数的类，不管有多少个虚函数，只有一个虚指针的大小
 * @version 0.1
 * @date 2022-07-07
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <iostream>

using namespace std;

class A{
    virtual void fun();
    virtual void fun1();
    virtual void fun2();
    virtual void fun3();
};

int main()
{
    cout << sizeof(A) << endl;
    return 0;
}