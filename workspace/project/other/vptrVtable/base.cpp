/**
 * @file base.cpp
 * @author your name (you@domain.com)
 * @brief C++虚函数和vptr和vtable
 * @version 0.1
 * @date 2022-07-07
 * 
 * @copyright Copyright (c) 2022
 * 
 */



#include <iostream>
#include <stdio.h>

using namespace std;


/**
 * @brief 函数指针
 * 
 */
typedef void (*Fun)();


/**
 * @brief 基类
 * 
 */
class Base
{
    public:
        Base(){}
        virtual void fun1()
        {
            cout << "Base::fun1()" << endl;
        }
        virtual void fun2()
        {
            cout << "Base::fun2()" << endl;
        }
        virtual void fun3() {}
        ~Base(){}
};

/**
 * @brief 派生类
 * 
 */

class Derived : public Base
{
    public:
        Derived(){}
        void fun1()
        {
            cout << "Derived::fun1()" << endl;
        }
        void fun2()
        {
            cout << "DerivedClass::fun2()" << endl;
        }
        ~Derived(){}
};

/**
 * @brief 获取vptr地址与func地址
 * vptr地址指向的是一块内存，这块内存存放的是虚函数地址
 * 这块内存就是我们说的虚表
 * 
 * @param obj 
 * @param offset 
 * @return Fun 
 */
Fun getAddr(void * obj, unsigned int offset)
{
    cout << "==============" << endl;
    void * vptrAddr = (void *)*(unsigned long *)obj; //64位操作系统，占8字节。
    //通过*(unsigned long *)obj 取出前8字节，即vptr指针
    printf("vptrAddr:%p\n",vptrAddr);

    /**
     * @brief 通过vptr指针访问虚表
     * 因为虚表中每个元素在64位下是8个字节，加上偏移量就是每个函数的地址
     * 
     */
    void * funcAddr = (void *)*((unsigned long *)vptrAddr+offset);

    printf("funcAddr:%p\n",funcAddr);

    return (Fun)funcAddr;
}

int main(void)
{
    Base ptr;
    Derived d;
    Base * pt = new Derived(); // 基类指针指向派生类实例
    Base & pp = ptr; //基类引用指向基类实例
    Base & p = d; //基类引用指向派生类实例

    cout << "基类对象直接调用" << endl;
    ptr.fun1(); 
    cout << "基类对象调用基类实例" << endl;
    pp.fun1();
    cout << "基类指针指向基类实例并调用虚函数" << endl;
    pt->fun1();
    cout << "基类引用指向基类实例并调用虚函数" << endl;
    p.fun1();

    //手动查找vptr和vtable
    Fun f1 = getAddr(pt,0);
    (*f1)();
    Fun f2 = getAddr(pt,0);
    (*f2)();
    delete pt;
    return 0;
}