/**
 * @file rule6.cpp
 * @author your name (you@domain.com)
 * @brief 虚继承
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
    virtual void fun() {}
};

class B
{
    virtual void fun2() {}
};

class C : virtual public A, virtual public B
{
    public:
        virtual void fun3() {}
};



int main()
{
    /**
     * @brief 8 8 16 派生类继承多个虚函数，会继承所有虚函数的虚指针
     * 
     */

    cout << sizeof(A) << " " << sizeof(B) << " " << sizeof(C);
    return 0; 
}