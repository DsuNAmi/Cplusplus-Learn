# decltype

## 基本使用

语法：

```c++
decltype (expression)
```

这里的括号是必不可少的，`decltype`的作用是**查询表达式的类型**，因此，上面语句的效果是，返回`experssion`表达式的类型。注意，`decltype`仅仅**查询**表达式的类型，并不会对表达式进行**求值**。

### 推导出表达式类型

```c++
int i = 4;
decltype(i) a; //推导结果为int。a的类型为int。
```



### 与using/typedef合用，用于定义类型

```c++
using size_t = decltype(sizeof(0)); // sizeof(a)的返回值类型为size_t类型
using ptrdiff_t = decltype((int *)0 - (int *)0);
using nullptr_t = decltype(nullptr);

vector<int> vec;

typedef decltype(vec.begin()) vectype;

for(vectype i = vec.begin(); i != vec.end(); ++i)
{
    ...;
}
```

这里和`auto`一样，也提高了代码的可读性。

### 重用匿名类型

```c++
struct 
{
    int d;
    double b;
}anon_s;
```

借助`decltype`，我们可以重新使用这个匿名的结构体

```c++
decltype(anon_s) as;
```

### 泛型编程中结合`auto`，用于追踪函数的返回值类型

```c++
template <typename T>
auto multiply(T x, T y)->decltype(x * y)
{
    return x * y;
}
```



## 判别规则

对于`decltype(e)`而言，其判别结果受以下条件的影响：

如果`e`是要给没有带括号的标记符表达式或者类成员访问表达式，那么`decltype(e)`就是`e`所命名的实体的类型。此外，如果`e`是一个被重载的函数，则会导致编译错误。否则，假设`e`的类型是`T`，如果`e`是一个将亡值，那么`decltype(e)`为`T&&`。否则，假设`e`的类型是`T`，如果`e`是一个左值，那么`decltype（e）`为`T&`。否则，假设`e`的类型是`T`，则`decltype(e)`为`T`。

标记符指的是除去关键字，字面量的编译器需要使用标记之外的程序员自己定义的标记，而单个标记符对应的表达式即为标记符表达式。例如：

```c++
int arr[4]
```

则`arr`为一个标记符表达式，而arr[3] + 0不是。

举例：

```c++
int i = 4;
int arr[5] = { 0 };
int * ptr = arr;
struct S{double d;} s;
void Overloaded(int);
void Overloaded(char);
int && RvalRef();
const bool Func(int);


//规则1：推导为其类型
decltype (arr) var1; //int[] 标记符表达式
decltype (ptr) var2; //int * 标记符表达式
decltype (s.d) var3; //double 成员访问表达式

//decltype(Overloaded) var4; //重载函数。编译错误。

//规则二：将亡值。推导为类型的右值引用
decltype (RvalRef()) var5 = 1;

//规则三：左值，推导为类型的引用
decltype ((i)) var6 = i; //int &
decltype (true ? i : i) var7 = i;//int & 条件表达式返回左值
decltype (++i) var8 = i; // int & ++i返回i的左值
decltype (arr[5]) var9 = i; //int & []操作返回左值
decltype (*ptr) var10 =i; // int & *操作返回左值
decltype ("hello") var11 = "hello"; //const char(&)[9] 字符串字面常量为左值，且为const左值。

//规则四：以上都不是，则推导为本类型
decltype (1) var12; // const int 
decltype (Func(1)) var13 = true; //const bool
decltype (i++) var14 = i; //int i++返回右值
```

