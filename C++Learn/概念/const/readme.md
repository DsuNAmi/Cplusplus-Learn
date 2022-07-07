# Const

## 含义

常类型是指使用类型修饰符`const`说明的类型，常类型的变量或对象是不能被更新的。

## 作用

- 可以定义常量

  ```c++
  const int a = 100
  ```

- 类型检查

  - `const`与`#define`的区别

    > `const`常量具有类型，编译器可以进行安全检查；`#define`宏定义没有数据类型，只是简单的字符串替换，不能进行安全检查。

  - `const`定义的变量只有类型为整数和或枚举，且以常量表达式初始化时才能作为常量表达式。

  - 其他情况下它只是一个`const`限定的变量，不要将与常量混淆。
  
- 防止修改，起保护作用，增加程序健壮性

  ```c++
  void f(const int i)
  {
      i++;//error!
  }
  ```

- 可以节省空间，避免不必要的内存分配

  - `const`定义常量从汇编的角度来看，只是给出了对应的内存地址，而不是像`#define`一样给出的是立即数。
  - `const`定义的常量在程序运行过程中只有一份拷贝，而`#define`定义的常量在内存中有若干个拷贝。

## const对象默认为文件局部变量

注意：非`const`变量默认为`extern`。要使用`const`变量能够在其他文件中访问，必须在文件中显式地指定它为`extern`。

> 未被`const`修饰地的变量在不同文件的访问

```c++
//file1.cpp
int ext;
//file2.cpp
#inlcude <iostream>

extern int ext;
int main()
{
    std::cout << (ext + 10) << std::endl;
}
```

> `const`常量在不同文件的访问

```c++
//extern_file1.cpp
extern const int ext = 12;
//extern_file2.cpp
#include <iostream>
extern const int ext;
int main()
{
    std::cout << ext << std::endl;
}
```

> 小结：
>
> 可以发现未被`const`修饰的变量不需要`extern`显式声明！而`const`常量需要显示声明`extern`，并且需要做初始化！因为常量定义后就无法修改，所以必须初始化。

## 定义常量

```c++
const int b = 10;
b = 0; // error: assingment of read-only variable 'b'
const string s = "helloworld";
const int i, j = 0; // error: uninitialized const 'i'
```

## 指针与Const

与指针相关的`const`有四种：

```c++
const char * a; //指向const对象的指针或者说指向常量的指针
char const * a; //同上
char * const a; //指向类型对象的指针。或者说常指针，const指针
const char * const a； //指向const对象的const指针
```

> 小结：
>
> 如果`const`位于`*`的左侧，则`const`就是用来修饰指针所指向的变量，即指针指向为常量。
>
> 如果`const`位于`*`的右侧，则`const`就是修饰指针本身，即指针本身是常量。

###### （1）指向常量的指针

```c++
const int * ptr;
*ptr = 10; //error
```

`ptr`是一个指向`int`类型`const`对象的指针，`const`定义的是`int`类型，而不是`ptr`本身，所以`ptr`可以不用赋初始值。但是不能通过`ptr`去修改所指对象的值。

除此之外，也不能用`void *`指针保存`const`对象的地址，必须使用`const void *`类型指针保存`const`对象的地址。

```c++
const int p = 10;
const void * vp = &p;
void * vp = &p //error
```

另外一个重点是，允许把非`const`对象的地址赋给指向`const`对象的指针。

```c++
const int * ptr;
int val = 3;
ptr = &val; //ok
```

我们不能通过`ptr`修改`val`的值。

> 小结：
>
> 1. 对于指向常量的指针，不能通过指针来修改对象的值。
> 2. 不能使用`void *`指针保存`const`对象的地址，必须使用`const void *`来保存`const`对象的地址。
> 3. 允许把非`const`对象的地址赋值给`const`对象的指针，如果要修改指针所指向的值，必须通过其他方式修改，不能直接通过当前指针直接修改。

###### （2）常指针

`const`指针必须进行初始化，且`const`指针的值不能修改。

```c++
#include <iostream>
using namespace std;

int main()
{
    int num = 0;
    int * const ptr = &num; //const指针必须初始化!且`const`指针的值不能修改
    int * t = &num;
    *t = 1;
    cout << *ptr << endl;
}
```

上述修改`ptr`指针所指向的值，可以通过非`const`指针来修改。

最后，当把一个`const`常量的地址赋值给`ptr`的时候，由于`ptr`指向的是一个变量，而不是`const`常量，所以会报错。出现`const int * -> int *`的错误。

```c++
#include <iostream>
using namespace std;
int main()
{
    const int num = 0;
    int * const ptr = &num; //error! const int * -> int *
    cout << *ptr << endl;
}
```

上述应该修改为：`const int * ptr`或者是`const int * const ptr`

###### （3）指向常量的指针

```c++
const int p = 3;
const int * const ptr = &p;
```

## 函数中使用Const

> `const`修饰函数返回值

###### （1）const int

```c++
const int func1();
```

这个本身无意义，因为参数返回本身就是赋值给其他的变量

###### （2）const int *

```c++
const int * func1();
```

指针指向的内容不变。

(3) int * const

```c++
int * const func2();
```

指针本身不可变

> `const`修饰修饰函数参数

(1) 传递过来的参数及指针本身在函数内不可变

```c++
void func(const int var); //传递过来的参数不可变
void func(int * const var); //指针本身不可变
```

表明参数在函数体内不能被修改。

输入参数采用“值引用”，由于函数将自动产生临时变量用于复制该参数，该输入参数本来就无需保护，所以不要加`const`修饰。

###### （2）参数指针所指内容为常量不可变

```c++
void StringCopy(char * dst, const char * src);
```

其中`src`是输入参数，`dst`是输出参数。给`src`加上修饰后，如果函数体内的语句试图改动`src`的内容，编译器将指出错误。

###### （3）参数为引用，为了增加效率同时防止修改

```c++
void func(const A & a);
```

对于非内部数据类型的参数而言，像`void func(A a)`这样声明的函数注定效率比较低。因为函数体内将产生A类型的临时对象用于复制参数a，而临时对象的构造、复制、析构都将消耗时间。

> 小结：
>
> 1. 对于非内部数据类型的输入参数，应该将**值传递**的方式改为`const`引用传递，目的是提高效率。例如将`void func(A a)`改为`void func(const A & a)`。
> 2. 对于内部数据类型的输入参数，不要将**值传递**的方式改为`const`引用传递。否则即达不到提高效率的目的，又降低了函数的可理解性。例如`void func(int x)`不应该改为`void func(const int & x)`。

## 类中使用Const

在一个类中，任何不会修改数据成员的函数都应该声明为`const`类型。如果在编写`const`成员函数时，不慎修改数据成员，或者调用了其他非`const`成员函数，编译器将指出错误，这无疑会提高程序的健壮性。

使用`const`关键字进行说明的成员函数，成为常成员函数。只有常成员函数才有资格操作常量或常对象，没有使用`const`关键字进行说明的成员函数不能用来操作常对象。

对于类中的`const`成员变量必须通过初始化列表进行初始化：

```c++
class Apple{
    private:
    	int people[100];
    public:
    	Apple(int i);
    	const int apple_number;
    
};

Apple:Apple(int i):apple_number(i)
{
    
}
```

`const`对象只能访问`const`成员函数，而非`const`对象可以访问任意的成员函数，包括`const`成员函数。

```c++
//apple.cpp
#include <iostream>

using namespace std;

class Apple
{
    private:
        int people[100];
    public:
        Apple(int i);
        const int apple_number;
        void take(int num) const;
        int add();
        int add(int num) const;
        int getCount() const;
};

Apple::Apple(int i):apple_number(i)
{

}

int Apple::add()
{
    take(1);
    return 0;
}

int Apple::add(int num) const
{
    take(num);
    return num;
}

void Apple::take(int num) const
{
    cout << "take func " << num << endl; 
}

int Apple::getCount() const
{
    take(1);
    add(); //error
    return apple_number;
}

int main()
{
    Apple a(2);
    cout << a.getCount() << endl;
    a.add(10);
    return 0;
}
```

此时报错，上面`getCount()`方法中调用了一个`add`方法，而`add`方法并非`const`修饰，所以运行报错。也就是说`const`成员函数只能访问`const`成员函数。

当调用改为：

```c++
const Apple b(3);
b.add(); //error
```

此时，可以证明的是，`const`对象只能访问`const`成员函数。

我们除了上述的初始化`const`常量用初始化列表方式外，还有如下方法：

###### 第一：将常量定义与`static`结合

```c++
static const int apple_numer;
```

###### 第二：在外面初始化

```c++
const int Apple::apple_number = 10;
```

如果使用`c++11 `进行编译，可以直接在定义初始化，可以直接写成：

```c++
static const int apple_number = 10;
//
const int apple_numer = 10;
```



