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

# 条款10，优先选用限定作用域的枚举型别，而非不限作用域的枚举型别

先说一个通用规则，如果在一对大括号里声明一个名字，则该名字的可见性就被限定在括号括起来的作用域内。



```c++
enum class Color {black, white, red};

auto white = false;			//可以，但是这个white并不是枚举类型

Color c = white;            //错误！范围内并无此枚举量

Color c = Color::white;     //可以

auto c = Color::white;      //可以
```



由于限定作用域的枚举型别是通过`enum class`声明的，所以有时候它们也被称为**枚举类**。其第一个优点就是**带来的名字空间污染降低**，第二个优点是**它的枚举量是更强型别的**，因为不限定范围的枚举类型是可以隐式转换到整数型别进一步转换到浮点型型别。

来看下面这个奇怪的例子：

```c++
enum Color {black, white, red};

std::vector<std::size_t> primeFactors(std::size_t);

Color c = red;

if(c < 14.5)        //枚举和浮点数比较
{
    auto factors = primeFactors(c);     //
}

//但是如果加了enum class 的话，上面的代码就都会报错
//如果是在需要转换的话，那么可以使用强制转换
if(static_cast<double>(c) < 14.5)
```



其第三个优点就是**可以进行前置声明**。同时也可以指定枚举型别的底层型别

```c++
enum class Status;		//底层型别是int
enum class Status: std::uint32_t;		//底层型别是std::uint_32t
enum class Status: std::uint32_t {
    good = 0,
    failed = 1,
    incomplete = 100,
    corrupt = 200,
    audited = 500,
    indeterminate = 0xFFFFFFFF
}
```



之后我们来看一个具体的案例，来比较两种方法的优缺点：

```c++
//不限定范围枚举
userInfo uInfo;

auto val = std::get<1>(uInfo);     //取用域1的值，但是我们有时候并不知道域1是什么，可以采用枚举

enum UserInfoFields {uiName, uiEamil, uiReputation};

auto val = std::get<uiEamil>(uInfo);        //这样就好多了

这是因为不限定范围枚举可以进行隐式转换，但是限定范围枚举就不行，只能进行强制转换

//限定范围枚举
    
userInfo uInfo;

enum class UserInfoFields {uiName, uiEmail, uiReputation};

auto val = std::get<static_cast<std::size_t>(UserInfoFields::uiReputation)>(uInfo);
```



###### 总结

- C++98风格的枚举型别，现在称为不限范围的枚举类型。
- 限定作用域的枚举型别仅在枚举型别内可见。它们只能通过强制型别转换以转换至其他型别。
- 限定作用域的枚举型别和不限范围的枚举型别都支持底层型别指定。限定作用域的枚举型别的默认底层型别是`int`，而不限范围的枚举型别没有默认底层型别。
- 限定作用域的枚举型别总是可以进行前置声明，而不限定范围的枚举型别却只有在指定了默认底层型别的前提下才可以进行前置声明。



# 条款11，优先选用删除函数，而非private未定义函数

如果你写了代码给其他程序员用，并且你想阻止他们调用某个特定函数的话，那你只需不要声明该函数即可。函数未经声明，不可调用。但是有时候C++会替你声明函数。

这种情况发生在**特种成员函数**上，即C++会在需要的时自动生成的成员函数。比如构造函数和复制构造函数。

在C++98中，采取的做法是使用`private`，并且不去定义它们。（**这个在另一本之前的书中有提到过**）

```c++
//C++98写法
template<class charT, class traits = char_traits<charT>>
class basic_ios : public ios_base
{
  	public:
    	...;
    private:
    	basic_ios(const basic_ios &);		//not defined
    	basic_ios& operator=(const basic_ios&);		//not defined
};

//c++11写法，使用删除函数
template<class charT, class traits = char_traits<charT>>
class basic_ios : public ios_base
{
  	public:
    	basic_ios(const basic_ios &) = delete;		
    	basic_ios& operator=(const basic_ios&) = delete;		
};
```



删除函数的一个优点是，任何函数都能成为删除函数，但只有成员函数才能是`private`。

我们来看一个例子，如果我们需要得到一个整数来判断是否是幸运数，但是我又需要保证只能接受整数，不支持隐式转换。

```c++
bool isLucky(int number) {return true;}

bool isLucky(bool) = delete;

bool isLucky(double) = delete;

bool isLucky(char) = delete;

//
isLucky(1);
isLucky(true);      //错误
isLucky('a');       //错误
```



删除函数还有一个能力就是能够阻止那些不应该进行的模板具现

```c++
template<typename T>
void processPointer(T* ptr);
//我们希望不能使用void*和char*
template<>
void processPointer(void*) = delete;
template<>
void processPointer(char*) = delete;
```



###### 总结

- 优先选用删除函数，而非`private`未定义函数。
- 任何函数都可以删除，包括非成员函数和模板具现。



# 条款12，为意在改写的函数添加override函数

C++11中的面向对象理念：在派生类中虚函数实现，会改写基类中对应虚函数的实现。

```c++
class Base
{
  	public:
    	virtual void doWork();
    	...;
};

class Derived : public Base
{
  	public:
    	virtual void doWork();		//virtual在这里可写可不写
    
};

std::unique_ptr<Base> upb = std::make_unique<Derived>();

upb->doWork();
```



如果要满足这个条件，需要：

- 基类中的函数必须是虚函数。
- 基类和派生类中的的函数名字必须完全相同（析构函数除外）。
- 基类和派生类中的函数形参型别必须完全相同。
- 基类和派生类中的函数常量性必须完全相同。
- 基类和派生类中的函数返回值和异常规格必须兼容。
- 基类和派生类中的函数引用饰词必须完全相同。

最后一条的例子：

```c++
class Widget {
    public:
    	void doWork() &;			//这个版本仅在*this是左值时使用
    	void doWork() &&;			//这个版本仅在*this时右值时使用
};


Widget makeWidget();			//工厂函数，返回右值

Widget w;						//普通对象（左值）

w.doWork();						//以左值调用
makeWidget().doWork();			//以右值调用

如果基类中的虚函数带有引用饰词，则派生类要对该函数进行改写版本必须也带有相同的引用饰词。
```



我们来看看`override`的作用。

```c++
class Base
{
    public:
        virtual void mf1() const;
        virtual void mf2(int x);
        virtual void mf3() &;
        void mf4() const;
};

class Derived : public Base
{
    //派生类的函数真的能够动态编译吗？
    public:
        virtual void mf1();
        virtual void mf2(unsigned int x);
        virtual void mf3() &&;
        void mf4() const;
};

//如果我们加上override
class Derived : public Base
{
    //可以看到四个函数全部报错。显示不能重写基类型
    public:
        virtual void mf1() override;
        virtual void mf2(unsigned int x) override;
        virtual void mf3() && override;
        void mf4() const override;
};

//修改
//如果我们加上override
class Derived : public Base
{
    //满足重写要求，没问题
    public:
        virtual void mf1() const override;
        virtual void mf2(int x) override;
        virtual void mf3() & override;
        void mf4() const override;  //在基类加virtual
};
```



如果我们想写一个函数，仅接受传入左值实参，则我们会声明一个非`const`左值引用形参

```c++
void doSomething(Widget & w);			//仅接受左值的Widget

void doSomething(Widget && w);			//仅接收右值实参


```



假设我们的类中有个`vector`成员，我们提供一个访问器函数能够让客户直接对这个数据成员进行访问。

```c++
class Widget
{
    public:
    	using DataType = std::vector<double>;
    	
    	DataType & data() {return values;}
   	private:
    	DataType values;
};

//客户使用
Widget w;
auto vals = w.data();			//把w.values复制到vals;

Widget::data的返回值类型是一个左值引用（std::vector<double>&）,并且因为左值引用定义为左值，我们实际上是使用一个左值来初始化vals的。这么一来，vals就是以w.values为基础进行复制构造的。;

//我们假设有一个工厂对象
Widget makeWidget();

auto vals2 = makeWidget().data();			//把Widget中的values复制到vals2

这里复制的是一个临时对象,临时对象如果是移动会更高效。;
```



解决方法

```c++
class Widget
{
    public:
    	using DataType = std::vector<double>;
    	...;
    	DataType & data() & {return values;}
    	DataType & data() && {return std::move(values);}
    private:
    	DataType values;
};

auto vals1 = w.data();			//调用左值版本，复制构造函数
auto vals2 = makeWidget().data();		//调用右值版本，采用移动构造
```



###### 总结

- 为意在改写的函数添加`override`声明。
- 成员函数引用饰词使得对于左值和右值对象（`*this`）的处理能够区分开来。



# 条款13，优先选用const_iterator，而非iterator

任何时候只要你需要一个迭代器而其涉指到的内容没有必要修改，就应该使用`const_iterator`。

C+11使用`const_iterator`：

```c++
std::vector<int> values;

auto it = std::find(values.cbegin(),values.cend(),1983);	//注意是cbegin,cend;
    
values.insert(it,1998);
```



###### 总结

- 优先选用`const_iterator`，而非`itreator`
- 在最通用的代码中，优先选用非成员函数函数版本的`begin,end,rbeing`，而非其成员函数版本。

# 条款14，只要函数不会发射异常，就为其加上noexcept声明

当你明明知道一个函数不会发射异常却未给函数加上`noexcept`声明的话，这就是接口规格缺陷。

```c++
int f(int x) throw();			//C++98风格,f不会发射异常
int f(int x) noexcept;			//C++11风格，f不会发生异常

如果，在运行期，一个异常逸出f的作用域，则f的异常规格被违反。;


RetType function(params) noexcept;			//最优化

RetType function(params) throw();			//优化不够

RetType function(params);					//优化不够
```



来看一个平常的例子：

```c++
Widget w;
std::vector<Widget> vw;
vw.push_back(w);

//C++98
如果空间不过，也就是size == capacity，这时候会把旧内存的内容移动到鑫的内存，然后析构旧内存的内容，如果这其中发生了异常，那么vector会保持不变，因为只有将所有旧内存数据转移到新内存，才会析构旧内存。;

//C++11
把C++98的复制操作改成了移动，这样会破坏原来的 强异常安全保证 。但是如果C++知道这个操作过程不会发生异常，那么就提高了效率。;

```



但是一个函数怎么知道移动操作不会产生异常呢？

**检验一下，看看这个操作是否带有noexcept**

```c++
//swap函数
template<class T, size_t N>
void swap(T (&a)[N], T(&b)[N]) noexcept(noexcept(swap(*a,*b)));

template<class T1, class T2>
struct pair{
    ...;
    void swap(pair & p) noexcept(noexcept(swap(first,p.first)) && noexcept(swap(second, p.second)));
};

这些函数带有noxcept声明，它们到底是不是具备noexcept性质，取决于它的noexcept分句中的表达式是否结果为noexcept。
```



大多数的函数都是中立的。此类函数自身并不抛出异常，但是它们调用的函数则可能发射异常。



###### 总结

- `noexcept`声明是函数接口的组成部分，这意味着带哦用方可能会对它有依赖。
- 相对于不带`noexcept`声明的函数，带有`noexcept`声明的函数可能有更多机会得到优化。
- `noexcept`性质对于移动操作、`swap`、内存释放函数和析构函数最有价值。
- 大多是函数都是**异常中立**的，不具备`noexcept`性质。



# 条款15，只要有可能使用constexpr，就使用它

当其应用于对象时，其实就是一个加强版的`const`。当应用于函数时，却有着不同的意义。

其含义：**它不仅是const，而且在编译阶段就已知**。



```c++
int sz;         //非constexpr变量

constexpr auto arraySize1 = sz;         //错误！sz的值在编译期未知

std::array<int,sz> data1;               //错误！一样的问题

constexpr auto arraySize2 = 10;         //10是一个编译期常量

std::array<int, arraySize2> data2;      //没问题，arraySize2 是一个constexpr

const auto arraySize = sz;          //没问题
std::array<int, arraySize> data3;       //编译期未知
```



所有`constexpr`对象都是`const`对象，而非所有的`const`对象都是`constexpr`对象。



如果涉及到`constexpr`函数，这样的函数在调用时若传入的时编译期常量，则产出编译其常量；如果传入的是直至运行时才知道的值，那么就产出运行期值。

理解：

- `constexpr`函数可以用在要求编译期常量的语境中。在这样的语境中，若你传给一个`constexpr`函数的实参值是在编译期已知的，则结果也会在编译期间计算出来。如果任何一个实参值在编译期未知，则你的代码将无法通过编译。
- 在调用`constexpr`函数时，若传入的值有一个或者多个在编译期未知，则它的运作方式和普通函数无异，亦即它也是在运行期间执行结果的计算。这意味着，如果函数执行的是同样的操作，仅仅应用的语境一个是要求编译期常量的，一个是用于所有其他值的话，那就不必写两个函数。`constexpr`函数就可以满足所有需求。



```c++
constexpr int pow(int base, int exp) noexcept       //pow是个constexpr函数，且不会抛出异常
{
    //C++11中，constexpr中不能包含超过一条语句
    return (exp == 0 ? 1 : base * pow(base,exp - 1));
    //C++14中没有这个限制
    auto result  = 1;
    for(int i = 0; i < exp; ++i) result *= base;
    return result;
}

constexpr auto numConds = 5;

std::array<int, pow(3,numConds)> results;       //results有3^numConds个元素
```



`constexpr`函数仅限于传入和返回字面型别，意思就是这样的型别能够持有编译期可以决议的值。在C++11中，所有的内建型别，除了`void`，都符合这个条件。

但是用户自定义型别同样也是适合这个要求：

```c++
class Point
{
    public:
        constexpr Point(double xVal = 0, double yVal = 0) noexcept
        :x(xVal), y(yVal) {}
        
        constexpr double xValue() const noexcept {return x;}
        constexpr double yValue() const noexcept {return y;}

        constexpr void setX(double newX) noexcept {x = newX;}
        constexpr void setY(double newY) noexcept {y = newY;}

    private:
        double x, y;
};

Point被声明成了constexpr函数，由于传入它的实参在编译期间可知，构造的Point也是在编译期可知;

constexpr Point p1(9.4,27.4);
constexpr Point p2(28.8,5.3);

constexpr Point midpoint(const Point & p1, const Point & p2) noexcept
{
    return {(p1.xValue() + p2.xValue()) / 2,
            (p1.yValue() + p2.yValue()) / 2};       //调用constexpr成员函数
    
}

constexpr auto mid = midpoint(p1,p2);		//你把鼠标放上去，编译器就把值算出来了。

//中心对称
constexpr Point reflection(const Point & p) noexcept
{
    Point result;

    result.setX(-p.xValue());
    result.setY(-p.yValue());


    return result;
}

constexpr auto reflectionMid = reflection(mid);
```



###### 总结

- `constexpr`对象都具备`const`属性，并由编译期已知的值完成初始化。
- `constexpr`函数在调用时若传入的实参值时编译期已知的，则会产生编译期结果。
- 比起非`constexpr`对象或`constexpr`函数而言，`constexpr`对象或是`constexpr`函数可以用在一个作用域更广的语境中。

# 条款16，保证const成员函数的线程安全性

来看一个例子

```c++
class Polynomial
{
    public:
        using RootType = std::vector<double>;

        RootType roots() const
        {
            if(!rootsAreValid)
            {
                rootsAreValid = true;
            }
            return rootVals;
        }

    private:
        mutable bool rootsAreValid{false};
        mutable RootType rootVals {};
};

从概念上说，roots操作不会改变它操作的Polynomial对象，然而作为缓存活动的组成部分，它可能需要修改rootVals和rootAreValid的值。这是mutable的经典用例。;

//设想现在有两个线程同时在同一个polynomial对象上调用roots
Polynomial p;

//在本例中，会发生 数据竞险，这段代码存在未定义行为。
auto rootsOfP = p.roots();
auto valsGivingZero = p.roots();

//解决方法1
class Polynomial
{
    public:
        using RootType = std::vector<double>;

        RootType roots() const
        {
            std::lock_guard<std::mutex> g(m);       //加上互斥量
            if(!rootsAreValid)
            {
                rootsAreValid = true;
            }
            return rootVals;
        }

    private:
        mutable std::mutex m;
        mutable bool rootsAreValid{false};
        mutable RootType rootVals {};
        
};
```



我们在来看一个例子：计算一个成员函数被调用的次数，使用`std::atomic`型别的计数器。

```c++
class Point
{
    public:
        double distanceFromOrigin() const noexcept
        {
            ++callCount;        //带原子性的自增操作

            return sqrt((x * x) + (y * y));
        }

    private:
        mutable std::atomic<unsigned> callCount {0};
        double x, y;
};

由于对std::atomic型别的变量的操作于加上与解除互斥量相比，开销往往比较小;
```



如果是某类需要缓存计算开销较大的`int`型别的变量，则应该尝试使用一对`std::atomic`型别来取代互斥量。

```c++
class Widget
{
    public:
        int magicValue() const
        {
            if (cachedValid) return cachedValue;
            else
            {
                auto val1 = expensiveComputation1();
                auto val2 = expensiveComputation2();
                cachedValue = val1 + val2;
                cachedValid = true;
                return cachedValue;
            }
        }

    private:
        mutable std::atomic<bool> cachedValid{false};
        mutable std::atomic<int> cachedValue;
};

但是上述写法会出现一种情况，如果一个线程发现cachedValid为false，则需要进行一段大开销计算两个值，同时另一个线程也执行该段代码，发现cachedValid也为false，则也会执行一大段开销计算值。;

//这时候就只能加上互斥量了
class Widget
{
    public:
        int magicValue() const
        {
            std::lock_guard<std::mutex> g(m);

            if (cachedValid) return cachedValue;
            else
            {
                auto val1 = expensiveComputation1();
                auto val2 = expensiveComputation2();
                cachedValue = val1 + val2;
                cachedValid = true;
                return cachedValue;
            }
        }

    private:
        mutable std::mutex m;
        mutable std::atomic<bool> cachedValid{false};
        mutable std::atomic<int> cachedValue;
};
```



###### 总结

- 保证`const`成员函数的线程安全性，除非可以确信它们不会用在并发语境中
- 运用`std::atomic`型别的变量会比运用互斥量提供更好性能，但前者仅仅使用对单个变量或内存区域的操作。



# 条款17，理解特种成员函数的生成机制

在C++中，特种函数是指那些C++会自行生成的函数。

c++11中的特种函数：

- 默认构造函数
- 析构函数
- 复制构造函数
- 复制赋值构造函数
- 移动构造函数，`Widget(Widget && rhs)`
- 移动赋值构造函数，`Widget & operator=(Widget && rhs)`



按成员移动是由两部分组成的，一部分是在支持移动操作的成员上执行的移动操作，另一部分是在不支持移动操作的成员上执行的复制操作。



大三律（Rule of Three）:

如果你声明了复制构造函数，复制赋值构造函数或析构函数的任何一个，你就得同时声明所有这三个。



移动操作的生成条件，仅当以下三者同时成立：

- 该类未声明任何复制操作。
- 该类未声明任何移动操作。
- 该类未声明任何析构函数。



```c++
class Widget
{
    public:
        ~Widget();

        Widget(const Widget &) = default;       //默认复制构造函数的行为是正确的

        Widget & operator=(const Widget &) = default; //同理
};
```



C++支配特种成员函数的极值如下：

- **默认构造函数**：与C++98相同。仅当类中不包含用户声明的构造函数时才生成。
- **析构函数**：与C++98的机制基本相同，唯一的区别在于析构函数默认为`noexcept`。仅当基类的析构函数为虚的，派生类的析构函数才是虚的。
- **复制构造函数**：运行期行为与C++98相同：按成员进行非静态数据成员的复制构造。仅当类中不包含用户声明的复制构造函数时才生成。如果该类声明了移动操作，则复制构造函数将被删除。在已经存在复制赋值构造函数运算符或析构函数的条件下，仍然生成复制构造函数已经成为了废弃的行为。
- **复制赋值运算符**：运行期行为与C++98相同：按成员进行非静态数据成员的复制赋值。仅当类中不包含用户声明的复制赋值运算符时才生成。如果该类声明了移动操作，则复制构造函数将被删除。在已经存在复制构造函数或析构函数的条件下，仍然生成复制赋值运算符已经成为了废弃的行为。
- **移动构造函数和移动赋值运算符**：都按成员进行非静态数据成员的移动操作。仅当类中不包含用户声明的复制操作、移动操作和析构函数才生成。



###### 总结

- 特种成员函数是指那些C++会自行生成的函数：默认构造函数、析构函数、复制操作、以及移动操作。
- 移动操作仅当类中未包含用户显式声明的复制操作、移动操作和析构函数时才生成。
- 复制构造函数仅当类中不包含用户显式声明的复制构造函数时才生成，如果该类中声明了移动操作则复制构造函数将被删除。复制赋值构运算符仅当类中不包含用户显式声明的赋值赋值运算符才生成，如果该类中声明了移动操作，则复制赋值运算符将被删除。在已经存在显式声明的析构函数的条件下，生成复制操作已经成为了被废弃的行为。
- 成员函数模板在任何情况下不会抑制特种函数的生成。