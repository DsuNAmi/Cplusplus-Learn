# 深入浅出的C++虚函数的vptr与vtable

## 基础理论

为了实现虚函数，c++使用一种称为虚拟表的特殊形式的后期绑定。该比虚拟表是用于解决在动态/后期绑定方式的函数调用的查找表。虚拟表有时候会使用其他名称，例如：`vtable`，虚拟数表，虚方法表，或者是调度表



虚拟表实际上非常简单，虽然用文字描述有点复杂。首先，**每个使用虚函数的类（或者从使用虚函数的类派生）都有自己的虚拟表**。该表只是编译器在编译时设置的静态数组。虚拟表包含可以由类的对象调用的每个虚拟函数的一个条目。此表中的每个条目只是一个函数指针，指向该类可访问的派生函数。



其次，编译器还会添加一个隐藏指向基类的指针，我们称之为`vptr`。`vptr`在创建类实例时自动设置，以便指向该类的虚拟表。与`this`指针不同，`this`指针实际上是编译器用来解析自引的函数参数，`vptr`是一个真正的指针。



因此，它使每个类对象的分配大一个指针的大小。这也意味着`vptr`由派生类继承，这很重要。

## 实现与内部结构

```c++
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
```

运行结果：

```bash
基类对象直接调用
Base::fun1()
基类引用指向派生类实例
Base::fun1()
基类指针指向派生类实例并调用虚函数
Derived::fun1()
基类引用指向基类实例并调用虚函数
Derived::fun1()
=======================
vptr_addr:0x401130
func_addr:0x400ea8
Derived::fun1()
=======================
vptr_addr:0x401130
func_addr:0x400ed4
DerivedClass::fun2()
```

我们发现C++的动态多态是通过虚函数来实现的。简单的说，通过`virtual`函数，指向子类的基类指针可以调用子类的函数。

```c++
Base * pt = new Derived();
cout << "output" << endl;
pt->fun1();
```

其过程为：

> 首先程序识别出`fun1()`是个虚函数，其次程序使用`pt->vptr`来获取`Derived`的虚表。接着，它查找`Derived`虚表中调用哪个版本的`fun1()`。这里就可以发现调用的是`Derived::fun1()`。之后被解析。



