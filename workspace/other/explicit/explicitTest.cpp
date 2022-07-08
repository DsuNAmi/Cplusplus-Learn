#include <iostream>

using namespace std;

struct A
{
    A(int) {}
    operator bool() const {return true;} //转换成bool类型
};


struct B
{
    explicit B(int) {}
    explicit operator bool() const {return true;}
};


void doA(A a) {}

void doB(B b) {}


int main()
{
    A a1(1); //直接初始化
    A a2 = 1; // 复制初始化
    A a3{1}; //直接列表初始化
    A a4 = { 1 }; //复制列表初始化
    A a5 = (A)1; //允许static_cast的显示转换
    doA(1); //允许 int 到 A的隐式转换
    if(a1) //使用函数A::operator bool() 的从A 到 bool 的隐式转换
    bool a6(a1); //使用函数转换
    bool a7 = a1;// 使用函数转换
    bool a8 = static_cast<bool>(a1); //static_cast直接进行初始化

    B b1(1); //OK
    // B b2 = 1; //error

    return 0;
}