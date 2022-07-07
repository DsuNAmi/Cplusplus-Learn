# sizeof

## 类大小计算

- 空类的大小为1字节。
- 一个类中，虚函数本身、成员函数（包括静态于非静态）和静态数据成员都是不占用类对象的存储空间。
- 对于包含虚函数的类，不管有多少个虚函数，只有一个虚指针`vptr`的大小。
- 普通继承，派生类继承了所有基类的函数和成员，要按照字节对齐来计算。
- 虚函数继承，不管是单继承还是多继承，都是继承了基类的`vptr`。（32位操作系统4字节，64位操作系统8字节）
- 虚继承，继承基类的`vptr`。

### 原则1

```c++
/**
 * @file blankclass.cppp
 * @brief 空类的大小为1字节
 * @version v1
 * @date 2022-07-07
 * 
 * @copyright Copyright (c) 2022
 * 
 */


#include <iostream>

using namespace std;

class A
{};


int main()
{
    cout << sizeof(A) << endl;
    return 0;    
}
```

### 原则2

```c++
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
```

### 原则3

```c++
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
```

### 原则4和原则5

```c++
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

```

### 原则6

```c++
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
```



