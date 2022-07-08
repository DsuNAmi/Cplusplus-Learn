# ::范围解析运算符

- 全局作用符 `::name`：用于类型名称前，表示作用域为全局命名空间
- 类作用符 `class::name`：用于表示指定类型的作用域范围是某个具体的类
- 命名空间作用域符 `namespace::name`：用于表示指定类型的作用域范围是具体某个命名空间的

```C++
/**
 * @file rangeTest.cpp
 * @author your name (you@domain.com)
 * @brief ::作用域解析符
 * @version 0.1
 * @date 2022-07-08
 * 
 * @copyright Copyright (c) 2022
 * 
 */



#include <iostream>
using namespace std;

int count = 0;

class A
{
    public:
        static int count; //类A的count
};

int A::count;

int main()
{
    ::count = 1;//设置全局的count
    A::count = 5;
    int count = 3;
    count = 4;
    cout << A::count << endl;
    cout << count << endl;
    cout << ::count << endl;
    return 0;
}
```
