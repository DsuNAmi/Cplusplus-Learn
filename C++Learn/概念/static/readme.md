# Static

当与不同类型一起使用时，`static`关键字具有不同的含义。

- **静态变量**：函数中的变量，类中的变量。
- **静态类的成员**：类对象和类中的函数。

## 静态变量

- 函数中的静态变量

当变量声明为`static`时，空间**将在程序的声明周期内分配**。即使多次调用该函数，静态变量的空间也只分配一次，前一次调用中的变量值通过下一次函数调用传递。这对于在C++或需要存储先前函数状态的任何其他程序非常有用。

```c++
#include <iostream>
#include <string>

using namespace std;

void demo()
{
    //static variable
    static int count = 0;
    cout << count << " ";
    // value is update and
    // will be carried to next
    // function calls
    count++;
}



int main()
{
    for(int i = 0; i < 5; ++i)
    {
        demo();
    }
    return 0;
}
```

输出：

```bash
0 1 2 3 4
```

您可以在上面的程序中看到`count`被声明为`static`。因此，它的值通过函数调用来传递。每次调用函数时，都不会对变量技计数进行初始化。

- 类中的静态变量

由于声明为`static`的变量只被初始化一次，因为它们在单独的静态存储中分配了空间，因此类中的静态变量**由对象共享**。对于不同的对象，不能有相同静态变量的多个副本。也是因为这个原因，静态变量不能使用构造函数初始化。

```c++
#include <iostream>

using namespace std;


class Apple
{
    public:
        static int i;

        Apple()
        {
            //Do nothing
        };
};

int main()
{
    Apple obj1;
    Apple obj2;
    obj1.i = 2;
    obj2.i = 3;

    cout << obj1.i << " " << obj2.i;
}
```

您可以在上面程序 中看到我们已经尝试为多个对象创建静态变量`i`的多个副本。但这并不会发生。因此，类中的静态变量应由用户使用类外的类名和范围解析运算符显式初始化，如下：

```c++
#include <iostream>

using namespace std;


class Apple
{
    public:
        static int i;

        Apple()
        {
            //Do nothing
        };
};

int Apple::i = 1; //init

int main()
{
    Apple obj1;
    Apple obj2;
    obj1.i = 2;
    obj2.i = 3;

    cout << obj1.i << " " << obj2.i;
}
```

输出：

```c++
3 3
```

## 静态成员

- 类对象为静态

就像变量一样，对象也在声明为`static`时具有范围，知道程序的生命周期。

考虑以下程序，其中对象也是非静态的。

```c++
#include <iostream>

using namespace std;

class Apple
{
    int i;
    public:
        Apple()
        {
            i = 0;
            cout << "Inside Constructor" << endl;
        }

        ~Apple()
        {
            cout << "Inside Destructor" << endl;
        }
};


int main()
{
    int x = 0;
    if(!x)
    {
        Apple obj;
    }
    cout << "End of main" << endl;
}
```

输出

```c++
Inside Constructor
Inside Destructor
End of main
```

在上面的程序中，对象在`if`块内声明为非静态。因此，变量的范围仅在`if`块内。当创建对象时，将调用构造函数，并且在`if`块的控制权越过析构函数的同时调用，因为对象的范围仅在声明它的`if`块内。如果我们改为静态，看看输出的变化：

```c++
#include <iostream>

using namespace std;

class Apple
{
    int i;
    public:
        Apple()
        {
            i = 0;
            cout << "Inside Constructor" << endl;
        }

        ~Apple()
        {
            cout << "Inside Destructor" << endl;
        }
};


int main()
{
    int x = 0;
    if(!x)
    {
        static Apple obj;
    }
    cout << "End of main" << endl;
}
```

输出：

```c++
Inside Constructor
End of main
Inside Destructor
```

- 类中的静态函数

就像类中的静态数据成员一样，静态成员函数也不依赖于类的对象。我们被允许使用对象和`.`来调用静态成员函数。但建议使用类名和范围解析运算符调用静态成员。

允许静态成员函数仅访问静态数据成员或者其他静态成员函数，它们无法访问类的非静态数据成员或成员函数。

```c++
#include <iostream>

using namespace std;

class Apple
{
    public:
        //sattic member function
        static void printMsg()
        {
            cout << "Welcome to Apple!";
        }
};


int main()
{
    //invoking a static member function
    Apple::printMsg();
}
```

输出：

```c++
Welcome to Apple!
```

限定访问范围，`static`还有限定访问范围的作用。

```c++
//source1.cpp
extern void sayHello();
const char * msg = "Hello World!\n";

int main()
{
    sayHello();
    return 0;
}

//source2.cpp
#include <cstdio>

extern char * msg;

void sayHello()
{
    printf("%s",msg);
}
```

`g++`对于上面两个代码文件是可以正常编译并且打印`Hello world!`，但如果给`source1.cpp`中的`msg`加上`static`，则会导致未定义的引用。