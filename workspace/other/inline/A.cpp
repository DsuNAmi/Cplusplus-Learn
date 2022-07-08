#include "A.h"
#include <iostream>

using namespace std;

/**
 * @brief inline 要起作用，inline要与函数定义放在一起，inline是一种“用于实现的关键字，而不是用于声明的关键字”
 * @param x 
 * @param y 
 * 
 * @return 
 */
int Foo(int x, int y);//函数声明
inline int Foo(int x, int y)
{
    return x + y;
}

inline void A::f1(int x)
{

}

int main()
{
    cout << Foo(1,2) << endl;
}

/**
 * 编译器对 inline 函数的处理步骤
 * 将inline函数体复制到inline函数调用点处
 * 为所用inline函数中的局部变量分配内存空间
 * 将inline函数的输入参数和返回值映射到调用方法的局部变量空间中
 * 如果inline函数有多个返回点，将其转变为inline函数代码块末尾的分支
 * 
 */