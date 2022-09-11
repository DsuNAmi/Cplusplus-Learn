# 条款7，在创建对象时注意区分()和{}

在C++11中，指定初始化值的方式包括小括号、使用等号、或者是大括号。

```c++
int x(0);

int y = 0;

int z{0};
//或者
int z = {0};
//以上两种写法是等同的。
```



来看一个和赋值相关的误区

```c++
Widget w1;			//调用默认构造函数

Widget w2 = w1;		//调用的是复制构造函数

w1 = w2;			//调用的是复制赋值构造函数
```



C++11引入了统一初始化：单一的、至少从概念上可以用于一切场合、表达一切意思的初始化。其基础形式是大括号。

```c++
std::vetor<int> v({1,3,5});			//v的初始内容为{1，3，5}

class Widget
{
    ...;
    private:
    	int x{0};		//可行，x的默认值为0
    	int y = 0;		//也可以
    	int z(0);		//不行
}

//如果是不可复制的对象，是能不能用=来进行初始化的。
std::atomic<int> ai1{0};
std::atomic<int> ai2(0);
std::atomic<int> ai3 = 0;		//不可行


```



大括号初始化有一项新特性，就是它禁止内建型别之间进行隐式窄化型别转换。

```c++
double x, y, z;
...;
int sum1{x + y + z};		//错误！double型别之和可能不能和int之间转换

//而小括号不会
int sum2(x + y + z);		//没问题，截断
int sum3 = x + y + z;		//同上

```



大括号初始化的另一项值得一提的特征是，它对于C++的**最令人苦恼之解析语法**免疫。

C++规定：任何能够解析为声明的都要解析为声明，而这会带来副作用。

意思就是，本来程序员想要以默认方式构造一个对象，结果去不小心声明了一个函数。

```c++
Widget w1(10);		//调用构造函数，参数为10

Widget w2();		//本来像用默认参数的构造函数，但是却声明成了函数

Widget w3{};		//调用默认参数的构造函数。
```



大括号初始化带来的问题就是C++中还有另外两个与其会混乱的东西：

1. `std::initializer_list`
2. 构造函数重载

```c++
class Widget
{
    public:
    	Widget(int i, bool b);
    	Widget(int i, double d);
    ...;
};

Widget w1(10,true);			//调用第一个构造函数
Widget w2{10,true};			//调用第一个构造函数
Widget w3(10,5.0);			//调用第二个构造函数
Widget w4{10,5.0};			//调用第二个构造函数


//如果有std::initializer_list
class Widget{
    public:
    	Widget(int i, bool b);
    	Widget(int i, double d);
    	Widget(std::initializer_list<long double> il);
    ...;
};

Widget w1(10,true);			//第一个构造函数
Widget w2{10,true};			//第三个构造函数，10和true被强制转换为long double
Widget w3(10,5.0);			//第二个构造函数
Widget w4{10，5.0};			//第三个构造函数，强制转换

//只有在实在找不到任何办法把大括号初始化物中的实参转化成std::initializer_list模板中的型别时，编译器才会退而去检查普通的重载协议。

class Widget
{
  	public:
    	Widget(int i, bool b);
    	Widget(int i, double d);
    	Widget(std::initializer_list<std::string> il);
};

Widget w1(10,true);			//第一个构造函数
Widget w2{10,true};			//第一个构造函数
Widget w3(10,5.0);			//第二个构造函数
Widget w4{10，5.0};			//第一个构造函数

//来看空参数的情况
class Widget
{
    public:
    	Widget();
    	Widget(std::initialilzer_list<int> il);
}

Widget w1;			//默认构造函数
Widget w2{};		//默认构造函数
Widget w3();		//函数声明
Widget w4({});		//空std::initializer_list<int>
Widget w5{{}};		//空std::initializer_list<int>
```



来看一个实际的例子：

```c++
std::vector<int> v1(10,20);		//10个元素，都是20
std::vector<int> v2{10,20};		//两个元素，分别时10，20
```



###### 总结

- 大括号初始化可以应用的语境最为宽泛，可以阻止隐式窄化型别转换，还对最令人苦恼之解析语法免疫。
- 在构造函数重载决议期间，只要有任何可能，大括号初始化物就会与带有`std::initializer_list`型别的形参相匹配，即使其他重载版本有着貌似更加匹配的形参表。
- 使用小括号还是大括号，会造成结果大相径庭的一个例子是：`std::vector`
- 在模板内容进行对象创建时，到底应该使用小括号还是大括号会成为一个棘手问题。

# 条款8，优先选用nullptr,而非0或NULL

字面常量0的型别是`int`，而非指针。当C++在只能使用指针的地方发现了一个0，它也会勉强解释为空指针，但说到底这是一个不得已而为止的行为。

**C++的基本观点还是0是int，而非指针**



`nullptr`的优点在于，它不具备整形型别，同时也不具备指针型别，但是却可以把它想象成一种任意型别的指针。

`nullptr`的实际型别是`std::nullptr_t`，并且，在一个漂亮的循环定义下，`std::nullptr_t`的定义被指定为`nullptr`的型别。型别`std::nullptr_t`可以隐式转换到所有的裸指针类别，这就是为什么`ullptr`可以扮演所有型别指针的原因。

因此，使用`nullptr`**避免了重载决议中的意外**。



我们来看一个利用`nullptr`的例子：假设你有一些函数，它们仅在适当的互斥量被锁定的前提下才能被调用，而每个函数的形参都是不同型别的指针。

```c++
int f1(std::shared_ptr<Widget> spw);		//仅当适当信息量被锁定
double f2(std::unique_ptr<Widget> upw);		//才调用这几个函数
bool f3(Widget * pw);


std::mutex fm1, fm2 ,fm3;			//f1,f2,f3对应的信息量

using MuxGuard = std::lock_gurad<std::mutex>;		//typedef

...;
{
    MuxGuard g(fm1);			//为f1锁定互斥量
    auto result = f1(0);		//向f1传递0作为空指针
    
}								//解锁互斥量
...;
{
    MuxGuard g(fm2);			//为f2锁定互斥量
    auto result = f2(NULL);		//向f2传入NULL作为空指针
}								//解锁互斥量
...;
{
    MuxGuard g(fm3);			//为f3锁定互斥量
    auto result = f3(nullptr);	//向f3传入nullptr作为空指针
}								//解锁互斥量


//以上代码有些冗余，将其模板化
template<typename FuncType,
		 typename MuxType,
		 typename PtrType>
auto lockAndCall(FuncType func, MuxType& mutex, PtrType ptr) -> decltype(func(ptr))
             //或者
             //decltype(auto) loackAndCall(...);	//替换上述写法
         {
             MuxGuard g(mutex);
             return func(ptr);
         }

auto result1 = lockAndCall(f1,fm1,0);		//错误，PtrType被解析为整形型别
auto result2 = lockAndCall(f2,fm2,NULL);	//错误，同理
auto result4 = lockAndCall(f3,fm3,nullptr);	//正确    
```



###### 总结

- 相对于0或者NULL，优先选用`nullptr`
- 避免在整形和指针型别之间重载。



# 条款9，优先选用别名声明，而非typedef

C++11提供了**别名声明**：

```c++
//c++98
typedef
std::unique_ptr<std::unordered_map<std::string, std::string>>
UPtrMapSS;

//C++11
using UPtrMapSS = 
    std::unique_ptr<std::unordered_map<std::string, std:string>>
```



两种方式定义函数指针别名

```c++
//typedef 
typedef void (*FP)(int, const std::string &);

using FP = void (*)(int, const std::string &);
```



其中**别名声明**可以模板化，而`typedef`不行

```c++
//typedef的嵌套写法
template<typename T>
struct MyAllocList{
    typedef std::list<T, MyAlloc> type;
    //含义为：MyAllocList<T>::type，是std::list<T,MyAlloc<T>>的同义词
};

MyAllocList<Widget>::type lw;		//客户代码

//别名声明
template<typename T>
using MyAllocList = std::list<T,MyAlloc<T>>;		
//含义为：MyAllocList<T>是std::list<T,MyAlloc<T>>的同义词

MyAllocList<Widget> lw;
```

###### 总结

- `typedef`不支持模板化，但别名声明支持。
- 别名模板可以让人免写`::type`后缀，并且在模板内，对于内嵌`typedef`的引用经常要求加上`typename`前缀。

