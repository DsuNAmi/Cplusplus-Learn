# RTTI(Runtime Type Identification)

c++有3个支持RTTI的元素。

- 如果可能的话，`dynamic_cast`运算符将使用一个指向基类的指针来生成一个指向派生类的指针；否则，该运算符返回空指针。
- `typdeid`运算符返回一个指出对象的类型的值。
- `type_info`结构存储了有关特定类型的信息。

**RTTI只适用于包含虚函数的类**

###### dynamic_cast

```c++
class Grand {/*has virtual method*/};
class Superb : public Grand {};
class Magnificent : public Superb {};


//假设有下面的指针
Grand * pg = new Grand;
Grand * ps = new Superb;
Grand * pm = new Magnificent;


//最后，对于下面的转换
Magnificent * p1 = (Magnificent *) pm;  //安全，因为是同类型转换
Magnificent * p2 = (Magnificent *) pg;  //不安全，基类转派生类，基类中不可能有派生类的内容
Superb * p3 = (Magnificent *) pm;		//安全，派生类赋值给基类。
```



先来看看`dynamic_cast`的语法：

```c++
Superb * pm = dynamic_cast<Superb *>(pg); //空指针，基类指针不能往派生类转
```



上述代码提出了一个问题：指针`pg`是否能够安全地转换到`Superb*`，如果可以，将返回运算对象的地址，否则返回空指针。

来看一个程序例子：

```c++
#include <iostream>
#include <cstdlib>
#include <ctime>

using namespace std;


class Grand
{
    private:
        int hold;
    public:
        Grand(int h = 0) : hold(h) {}
        virtual void Speak() const {cout << " I am a grand class! \n";}
        virtual int Value() const {return hold;}
};


class Superb : public Grand
{
    public:
        Superb(int h = 0) : Grand(h) {}
        void Speak() const {cout << "I am a superb class!! \n";}
        virtual void Say() const {cout << "I hold the superb value of " << Value() << "!\n";}
};


class Magnificent : public Superb
{
    private:
        char ch;
    public:
        Magnificent(int h = 0, char c = 'A') : Superb(h), ch(c) {}
        void Speak() const {cout << "I am a magnificent class!!!\n";}
        void Say() const {cout << "I hold the character " << ch << " and the integer " << Value() << "!\n";} 
};

Grand * GetOne();

int main()
{
    std::srand(std::time(0));
    Grand * pg;
    Superb * ps;
    for(int i = 0; i < 5; ++i)
    {
        pg = GetOne();
        pg->Speak();
        if(ps = dynamic_cast<Superb*>(pg))
        {
            ps->Say();
        }
    }

    return 0;
}

Grand * GetOne()
{
    Grand * p;
    switch(std::rand() % 3)
    {
        case 0:
            p = new Grand(std::rand() % 100);
            break;
        case 1:
            p = new Superb(std::rand() % 100);
            break;
        case 2:
            p = new Magnificent(std::rand() % 100,
                                'A' + std::rand() % 26);
            break;
    }

    return p;
}
```



判断转换的能否调用`Say()`。

###### typeid运算符和typeinfo类

`typeid`使得能够确定两个对象是否为同种类型。它与`sizeof`有点像，可以接收两种参数：

- 类名。
- 结果为对象的表达式。

`typeid`返回一个`type_info`对象的引用。

```c++
typeid(Magnificent) == typeid(*pg)
```



如果`pg`是一个`Magnificent`对象，那么为`true`，否则为`false`。

如果`pg`是空指针，那么会发生异常。

## 类型转换运算符

需要更严格地限制允许的类型转换，并添加4个类型转换运算符，使转换过程更加规范：

- `dynamic_cast`
- `const_cast`
- `static_cast`
- `reinterpret_cast`

###### const_cast

用于执行一种用途的转换，即改变值为`const`或`volatile`

```c++
const_cast<type-name> (experssion);
```



例子：

```c++
High bar;
const High * pbar = &bar;
High * pb = const_cast<High *>(pbar) ; //valid，删除const标签
const Low * pl = const_cast<const Low *> (pbar); //invalid，不是const之间的转换


//通过常规的方法也能转换，但是会不安全
High * pb = (High *)(pbar);
Low * pl = (Low *) (pbar); // also valid //同时涉及两种转换，派生类转基类
```



`const_cast`不是万能的，它可以修改指向一个值的指针，但修改`const`的值的结果是不确定的。

```c++
#include <iostream>

using namespace std;

void change(const int * pt, int n)
{
    int * pc;

    pc = const_cast<int *> (pt);
    *pc += n;
}

int main()
{
    int pop1 = 38383;
    const int pop2 = 200;
    cout << "pop1, pop2: " << pop1 << ", " << pop2 << endl;
    change(&pop1,-103);
    change(&pop2,-103);
    cout << "pop1, pop2: " << pop1 << ", " << pop2 << endl;

    return 0;
}
```



`pop2`的值不会被修改。



###### static_cast

```c++
static_cast<type-name> (experssion);
```



仅当`type-name`可被隐式转换为`experssion`所属的类型或`experssion`可被隐式转换为`type-name`所属的类型是，上述转换才是合法的。

如果`High`是`Low`的基类，`Pond`是一个无关的类

```c++
High bar;
Low blow;

High * pb = static_cast<High *> (&blow); 	//valid upcast
Low * p1 = static_cast<Low *> (&bar);		//valid downcast
Pond * pmer = static_cast<Pond *> (&blow);	//invali，

```

