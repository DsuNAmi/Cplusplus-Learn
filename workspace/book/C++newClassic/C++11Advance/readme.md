# C++11新特性

### 标准库function类型简介

头文件

```c++
#include <functional>
```



用法

```c++
int echovalue(int value)
{
    std::cout << value << std::endl;
    return value;
}


std::function<int(int)> f1 = echovalue;
    f1(5);


std::function<double(double)> f2 = [](double x)->double{
    std::cout << x << std::endl;
    return x;
};

f2(10);
```



### 万能引用

来看一个函数模板

```c++
template<typename T>
void func(const T & abc){}
func(10);
```

> 其中`T`的类型`int`，`abc`的类型是`const int &`。



要牢记：

**万能引用是一种类型**



```c++
template<typename T>
void myFunc(T && tmprv)
{
    //注意，&&是属于tmprv类型的一部分，不是T类型的一部分（&&和T类型没有关系）
    std::cout << tmprv << std::endl;
    return;
}
```

> 对于该段代码：
>
> - 这里的`tmprv`参数能接收左值，也能接收右值。
> - `tmprv`的类型是`T&&`。



所以，如果是万能引用的条件：

- 必须是函数模板。
- 必须是发生了模板类型推断并且函数模板类型为：`T&&`。

- 还有一种也是万能引用：`auto && someVar = ...`



`const`会剥夺一个引用成为万能引用的资格，被打回原型成为右值引用。

```c++
template<typename T>
class myTest
{
    public:
        void testfunc(T&& x) {}         //右值引用，因为不涉及推断，类创建的对象的时候已经确定了类型
        template<typename T2>
        void testfunc2(T2&&x){}         //万能引用，调用函数的时候还需要做推断
};
```

