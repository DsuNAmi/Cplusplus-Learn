# USING

## 基本使用

局部与全局`using`。

参考代码：`usingGlobal.cpp`

## 改变访问性

参考代码：`derivedBase.cpp`

```c++
#include <iostream>

class Base
{
    public:
        std::size_t size() const {return n;}
    protected:
        std::size_t n;
};

class Derived : private Base
{
    public:
        using Base::size;
    protected:
        using Base::n;
};
```

类`Derived`私有继承了`Base`，对于它来说成员变量`n`和成员函数都是私有的，如果使用了`using`语句，可以改变他们的可访问行。

上述例子中，`size`可以按照`public`的权限访问，`n`可以按`protected`的权限访问。

## 函数重载

在继承过程中，派生类可以覆盖重载函数的0个或多个实例，一旦定义了一个重载版本，那么其他的重载版本都会变为不可见。

如果对于基类的重载函数，我们需要在派生类中修改一个，又要让他的保持可见，必须要重载所有版本。

```c++
#include <iostream>

using namespace std;

class Base
{
    public:
        void f() {cout << "f()" << endl;}
        void f(int n) {cout << "Base::f(int)" << endl;} 
};

class Derived : private Base
{
    public:
        using Base::f;
        void f(int n) {cout << "Derived::f(int)" << endl;}
};

int main()
{
    Base b;
    Derived d;
    d.f();
    d.f(1);
    return 0;
}
```

如上述代码中，在派生类中使用`using`声明语句指定一个名字而不指定形参列表，所以一条基类成员函数的`using`声明语句就可以把该函数的所有重载实例添加到派生类的作用域中。此时，派生类只需要定义特有的函数就行了，而无需为继承而来的其他函数定义。

## 取代typedef

参考代码 `usingTypedef.cpp`