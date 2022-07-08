/**
 * @file pureVirtual.cpp
 * @author your name (you@domain.com)
 * @brief 纯虚函数
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
    private:
        int a;
    public:
        virtual void show() = 0;
};


int main()
{
    // 1. 抽象类只能作为基类来派生新类使用
    // 2. 抽象类的指针和引用指向由抽象类派生出来的类的对象

    //A a; //error, 抽象类，不能创建对象

    A * a1;

    // A * a2 = new A(); //error
}