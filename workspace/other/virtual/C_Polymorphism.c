/**
 * @file C_Polymorphism.cpp
 * @author your name (you@domain.com)
 * @brief C语言实现多态
 * @version 0.1
 * @date 2022-07-08
 * 
 * @copyright Copyright (c) 2022
 * 
 */


#include <stdio.h>

typedef void (*pf)();

/**
 * @brief 父类
 * 
 */
typedef struct A
{
    pf _f;
}A;

typedef struct B
{
    A _b; //在子类中定义一个基类的对象即可实现对父类的继承
}B;

void FunA()
{
    printf("%s\n","Base A::FunA()");
}

void FunB()
{
    printf("%s\n","Derived B::FunB()");
}

int main()
{
    A a;
    B b;

    a._f = FunA;
    b._b._f = FunB;

    A * pa = &a;
    pa->_f();
    pa = (A *)&b; //让父类指针指向子类的对象，由于类型不匹配所以要进行强制转换
    pa->_f();

    return 0;
}