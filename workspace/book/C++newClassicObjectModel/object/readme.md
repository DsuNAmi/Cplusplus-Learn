# 对象

### 类所占用的空间

```c++
class A
{
    public:

};


int main()
{
    A a;

    int ilen = sizeof(a);
    
    printf("ilen = %d\n", ilen);

    return 0;
}

//输出
//ilen = 1
```

> 对象`a`是有地址的。内存中的一个地址单元里面保存的是1个字节内容。
>
> 确切的含义是：**产生一个类A的对象时，这个对象所占用的内存大小是一个字节。**



如果我们增加3个成员函数

```c++
class A
{
    public:
        void func() {};
        void func1() {};
        void func2() {};
};
```

> 这里我们再算一个`sizeof(a)`，发现结果是1个字节。
>
> 得到一个结论：**类中的成员函数是不占用类对象内存空间的**

之后，我们向该类中添加一个成员变量：

```c++
class A
{
    public:
        void func() {};
        void func1() {};
        void func2() {};
    public:
        char ab;
};

```

> 此时再算一个`sizeof(a)`，还是1，不过这个1就是`char ab`的类型的一个字节。
>
> 如果改成`int ab`，那么`sizeof(a)`的值就是4。

所以我们能够得出

- 成员变量是包含在每个对象中的，占字节的。
- 成员函数虽然也写在类定义中，但成员函数不占类对象的字节空间。

### 对象结构的发展和演化

###### 非静态的成员变量跟着对象走

如果在类中定义了这个成员变量，那么`sizeof`这个类对象时，这个类对象占的内存就会因为这个成员变量的存在而增长。

###### 静态成员变量跟对象没有什么关系

```c++
class B
{
    static int a;
    static int b;
};

//使用sizeof输出就是1。不管几个静态成员变量都无所谓，sizoef的大小都不会增加。
```

###### 成员函数

不管静态的还是非静态的，全部都保存在类对象之外。所以不管几个成员函数，不管是否静态成员函数，对象的`sizeof`的值大小都不增加。

```c++
class C
{
    static void sfunc();
    void myfunc() {};
};
```

###### 虚函数

```c++
class D
{
    virtual void myfunc3();
    virtual void myfunc4();
};

//sizeof输出一个类的对象，大小为4（8），如果是64位的电脑就是8
```

> 类中因为有了多一个或者多个虚函数的存在，对象的`sizeof`会**增加**4（8）个字节。【要看电脑是32位还是64位，相当于一个指针的大小。】
>
> 因为一个类中有一个虚函数，就会产生一个指向虚函数的指针。多个虚函数的指针都放在一个**虚函数表**中。
>
> 无论往类中增加几个虚函数，类对象的`sizeof`只会增加4（8）个字节。这字节的增加，其实是因为虚函数的存在，导致系统往类中添加了一个指针，这个指针就正好指向这个虚函数表。

###### 总结

- 静态成员变量不计算在对象的`sizeof`内。

- 普通成员函数的静态成员函数不计算在`sizeof`内。

- 虚函数不计算在对象的`sizeof` 内，但虚函数会让对象的`sizeof`增加4（8）个字节以容纳指向虚函数表的指针。

- 虚函数表是跟着基于类的。

- 如果类中有多个数据成员，那么为了访问速度和性能考虑，某些编译器可能会将数据成员之间的内存占用比例进行调整。

  ```c++
  char a;		//1字节
  int  b;		//4字节
  ```

  > 两者本来占5字节，但是会因为字节对齐，给`char`分配四个字节。

- 不管是什么类型的指针，占用的内存大小是固定的。要么是4个字节要么是8个字节。

### this指针调整

`this`指针调整这件事，一般存在于多重继承的情况下。

```c++
class A
{
    public:
        int a;
        A()
        {
            printf("A::A()'s this pointer is : %p!\n", this);
        }
        void funcA(){printf("A funcA:this = %p\n", this);}
};

class B
{
    public:
        int b;
        B()
        {
            printf("B::B()'s this pointer is : %p!\n", this);
        }
        void funcB(){printf("B funcB:this = %p\n", this);}
};


class C : public A, public B
{
    public:
        int c;
        C()
        {
            printf("C::C()'s this pointer is : %p!\n",this);
        }
        void funcC(){printf("C funcC this = %p\n",this);}
};


int main()
{
    std::cout << sizeof(A) << std::endl;
    std::cout << sizeof(B) << std::endl;
    std::cout << sizeof(C) << std::endl;

    C myc;
    myc.funcA();
    myc.funcB();
    myc.funcC();

    return 0;
}

//输出如下
4
4
12
A::A()'s this pointer is : 00000030fe1ff804!
B::B()'s this pointer is : 00000030fe1ff808!
C::C()'s this pointer is : 00000030fe1ff804!
A funcA:this = 00000030fe1ff804
B funcB:this = 00000030fe1ff808
C funcC this = 00000030fe1ff804
```

> 1. 前面三行是分别的两个`int`类型，然后C = A + B + 4；
> 2. 之后是调用构造函数，要注意的是，A类的构造函数与B类的构造函数的`this`指针差了4个字节，但是C类的和A类的指针相同。
>
> 这要注意继承顺序，这里C是先继承的A，所以指针才会跟A一样。

如果一个派生类只从一个基类继承，那么，这派生类对象的地址和基类子对象地址相同。但如果派生类对象同时继承多个基类，那就要注意一下，第一个基类子对象的起始地址和派生类对象起始地址相同。后续的这些基类子对象的起始对象和派生类对象起始地址相差前面那些基类子对象所占用的内存空间。本例A类的空间占用是4个字节。

所以：**调用哪个子类的成员函数，这个this指针就会被编译器自动调整到对象内存布局中对应该类子对象的起始地址那里。**



