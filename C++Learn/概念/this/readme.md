# This

##### 用处

1. 一个对象的`this`指针并不是对象本身的一部分，并不会影响到`sizeof(object)`的结果。
2. `this`作用域在类内部，当在类的非静态成员函数中访问类的非静态成员的时候，编译器会自动将对象本身的地址作为一个隐含参数传递给函数。也就是说，即使你没有写上`this`指针，编译器在编译的时候也是会加上`this`的，它作为非静态成员函数的隐含形参，对各成员的访问均通过`this`进行。

##### 使用

1. 在类的非静态成员函数中返回类对象本身的时候，直接使用`return *this`。
2. 当参数与成员变量名相同的时候，如`this->n = n`（不能写成`n =n`）。

我们来看一个例子：

```c++
#include <iostream>
#include <cstring>

using namespace std;

class Person
{
    public:
        typedef enum{
            BOY = 0,
            GIRL
        }SexType;

        Person(char * n, int a, SexType s)
        {
            name = new char[strlen(n) + 1];
            strcpy(name,n);
            age = a;
            sex = s;
        }

        int get_age() const{
            return this->age;
        }

        Person& add_age(int a)
        {
            age += a;
            return *this;
        }

        ~Person()
        {
            delete [] name;
        }

    private:
        char * name;
        int age;
        SexType sex;

};



int main()
{
    Person p("zhangsan",20,Person::BOY);
    cout << p.get_age() << endl;
    cout << p.add_age(10).get_age() << endl;
    return 0;
}
```

总结：

> `this`在成员函数的开始执行前构造，在成员的执行结束后清除。上述的`get_age`函数会被解析`get_age(const A * const this)`，`add_age`函数会被解析成为`add_age(A * const this, int a)`。在C++中类和结构是只有一个区别：
>
> - 类的成员默认是`private`，而结构是`public`。`this`是类的指针，如果换成结构体，那么就是结构体的指针。

