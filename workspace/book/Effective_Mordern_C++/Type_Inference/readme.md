# 条款1，理解模板型别推导

我们可以先分析如下伪码：

```c++
template<typename T>
void f(paramType param);

//而一次调用如下
f(expr);			//以某表达式调用f
```

> 在编译期，编译器通过`expr`推导两个型别：一个是T的型别，另一个是`ParamType`的型别，这两个型别往往不一样。因为，`ParamType`常会包含一些修饰词，如`const`或引用符号等限定词。



比如：

```c++
template<typename T>
void f(const T& param);				//ParamType是const T&\

//如果有如下调用语句
int x = 0;
f(x);
//在此例中，T被推导为int, paramType被推导为const int&
```



`paramType`的类型要分为三种情况讨论：

- `paramType`具有指针或引用类型，但不是个万能引用。
- `paramType`是一个万能引用。
- `paramType`即非指针也非引用。



###### 第一种情况

运作情况分为两步：

1. 若`expr`具有引用型别，先将引用部分忽略。
2. 然后，对`expr`的型别和`paramType`的型别执行模式匹配，来决定`T`的型别。

我们的样例如下：

```c++
template<typename T>
void f(T& param);


//声明了如下变量：
int x = 27;
const int cx = x;
const int & rx = x;

//调用
f(x);			//T的型别是int, param的型别是int &
f(cx);			//T的型别是const int, param的型别是const int&
f(rx);			//T的型别是const int, param的型别是const int&
```

> 在第三个调用中，即使`rx`具有引用型别，`T`也没有被推导成一个引用。原因在于，`rx`的引用性（reference-ness）会在型别推导过程中被忽略。



其他满足上述情况的推导规则都相同。



###### 第二种情况

对于持有万能引用的形参而言，规则就不那么明显了。

- 如果`expr`是一个左值，`T`和`paramType`都会被推导为左值引用。这个结果具有双重的奇特之处。首先，这是在模板型别推导中，`T`被推导为引用型别的唯一情形。其次，尽管在声明时使用右值引用的语法，它的型别推导结果却还是左值引用。
- 如果`expr`是一个右值，则应对应情况1的规则。

```c++
template<typename T>
void f(T&& param);					//param现在是一个万能引用。

int x = 27;
const int cx = x;
const int & rx = x;


f(x);					//x是一个左值，所以T的类型是int&, param的类型也是int&
f(cx);					//cx也是一个左值，所以T的类型是const int &, param的类型也是
f(rx);					//都是const int &
f(27);					//27是一个右值，所以T的类型是int，而param的类型是int&&
```



###### 第三种情况

按值传递的情况，这意味着，无论传入的是什么，`param`都会是它的一个副本，也即一个全新的对象。

- 一如之前，若`expr`具有引用型别，则忽略其引用部分。
- 忽略`expr`的引用性之后，若`expr`是个`const`对象，也忽略之。若其是一个`voliate`对象，也要忽略。

```c++
template<typename T>
void f(T param);

int x = 27;
const int cx = x;
const int & rx = x;

f(x);
f(cx);
f(rx);
//三种传递，T和param都是int
```

> `param`是一个完全独立于`cx,rx`存在的对象，是一个副本。所以对于`cx,rx`不能修改这件事并不适用于`param`。



但是我们看一组特殊情况。

```c++
template<typename T>
void f(T param);

const char * const ptr = "Fun with pointes";

f(ptr);				//传递型别为const char * const的实参。

当ptr被传递给f时，这个指针本身将会按比特复制给param。换言之，ptr这个指针自己会被按值传递。依照按值传递形参的型别推导规则，ptr的常量性会被忽略，param被推导为const char *，即一个可修改的，涉及到一个const字符串的指针。
```



### 数组实参

数组型别有区别于指针类别，尽管有时它们看起来可以互换。形成这种假象的主要原因是：在很多语境下，数组会退化成指涉其首元素的指针。

```c++
//退化机制发生作用
const char name[] = "J. P. Briggs";			//name的型别是const char[13]

const char * ptrToName = name;				//数组退化成指针
```



但是当一个数组传递给持有按值形参的模板时，会发生什么呢？

```c++
template<typename T>
void f(T param);


f(name);			//在此例中，虽然name是一个数组，但是T的型别却被推导为了一个const char *

template<typename T>
void f(T & param);

f(name)l			//在此例中，T的型别会被推导为一个实际的数组型别！并且包含数组尺寸，所以推导的结果为：const char (&)[13]
```



有意思的是，可以利用声明数组引用这个能力创造出一个模板，用来推导出数组含有的元素个数。

```c++
template<typename T, std::size_t N>
constexpr std::size_t arraySize(T (&)[N]) noexcept		//返回值在编译期就可以用
{
    return N;
}

int keyVals[] = {1,3,7,9,11,22,35};		//keyVals含有7个元素

int mappedVals[arraySize(keyVals)];		//生成与其相同的数量
    
//或者
std::array<int, arraySize(keyVals)> mappedVals;	//长度也是7
```



### 函数参数

数组并非C++中唯一可以退化为指针之物。函数型别也同样会退化成函数指针，并且我们针对数组型别推导的一切讨论都适用于函数及其向函数指针的退化。

```c++
void someFunc(int ,double);			//someFunc是一个函数，型别为void (int,double)

template<typename T>
void f1(T param);

template<typename T>
void f2(T & param);


f1(someFunc);				//param被推导为函数指针，void(*)(int, double)

f2(someFunc);				//被推导为函数引用, void (&)(int, double);

```



###### 总结

- 在模板型别推导过程中，具有引用型别的实参会被当成非引用型别来处理。
- 对万能引用形参进行推导时，左值实参会进行特殊处理。
- 对按值传递的形参进行推导时。若实参中有`const,volatile`修饰词，则会忽略。
- 在模板型别推导过程中，数组或函数性别的实参会退化成对应的指针，除非他们被用来初始化引用。



# 条款2，理解auto型别推导

当某变量采用`auto`来声明时，`auto`就扮演了模板中`T`的这个角色，而变量的型别修饰词则扮演的是`paramType`的角色。

```c++
auto x = 27;
const auto cx = x;
const auto & rx = x;


//来看概念性模板，编译器并没有真正生成如下语句，只是逻辑上等价。

template<typename T>
void func_for_x(T param);

func_for_x(27);			//

template<typename T>
void func_for_cx(const T param);

func_for_cx(x);

template<typename T>
void func_for_rx(const T& param);

func_for_rx(x);
```



所以`auto`的使用也分三种情况：

- 型别修饰词是指针或引用，但不是万能引用。
- 型别修饰词是万能引用。
- 型别修饰词非指针也非引用。

同样也适用于退化情况。

```c++
auto x = 27;
const auto cx = x;
const auto & rx = x;

auto && uref1 = x;			//uref1的类型是int &
auto && uref2 = cx;			//const int &
auto && uref3 = 27;			//int &&

const char name[] = "J. P. Briggs";		//name的型别是name[13]

auto arr1 = name;			//const char *
auto & arr2 = name;			//const char (&)[13]
```



但是还有一处不同

```c++
//声明一个int类型的方法
int x1 = 27;
int x2(27);

int x3 = {27};
int x4{27};

//如果全部换成auto
auto x1 = 27;
auto x2(27);
auto x3 = {27};
auto x4{27};

前面两个语句都生成了一个类型为int,27的变量;
但是后面两个却声明了一个std::initializer_list<int>，且含有单个值为27的元素;

//所以
auto x = {11,23,9};			//x的类型是std::initializer_list<int>

template<typename T>
void f(T param);

f({11,23,9});			//推导失败

所以，auto与模板型别推导的真正唯一区别在于，auto会假定用大括号括起来的初始化表达式是一个std::initializer_list<T>类型，但模板不会。;
```



C++14允许使用`auto`来说明函数返回值需要推导，而C++14中的`lambda`表达式也会在形参中用到`auto`，这些`auto`用法是在使用模板推导而不是`auto`推导。

```c++
auto CreateInitList()
{
    return {1,2,3};			//无法完成推导。
}

//lambda表达式也是一样
std:;vector<int> v;

auto resetV = 
    [&v](const auto & newValue) {v = newValue;};

resetV({1,2,3});	//失效
```



###### 总结

- 在一般情况下，`auto`型别推导和模板i性别推导式一模一样的，但是在大括号情况下会有所不同。`std::initializer_list<T>`
- 在函数返回值或`lambda`式的形参中使用`auto`，意思是使用模板型别推导而不是`auto`型别推导。



# 条款3，理解decltype

`decltype`能够告诉你该名字或表达式的类型。

与`auto`和模板的型别推导过程相反，`decltype`一般只会鹦鹉学舌，返回给定的名字或表达式的确切类型。



在c++11中，`decltype`的主要用途大概就在于声明那些返回值型别依赖于形参型别的函数模板。

来看一个使用`decltype`来计算返回值类型。

```c++
template<typename Container, typename Index>		//能运作，但是需要改进
auto authAndAccess(Container& c, Index i) -> decltype(c[i])
{
    authenticateUser();
    return c[i];
}
```

> 这是一种**返回值型别尾序语法**，即该函数的返回值型别将在形参列表之后。尾序语法的好处在于，指定返回值型别时可以使用函数参数。
>
> 采用了这样的声明之后，`operator[]`返回值是什么类型，`authAndAccess`是什么类型，返回值就是什么类型。

可以看C++14的写法

```c++
template<typename Container, typename Index>
auto authAndAccess(Container & c, Index i)
{
    authenticateUser();
    return c[i];		//会根据c[i]推导出来
}

但是这样的写法是存在问题的。;


//来看这个例子
std::deque<int> d;
....;
authAndAccess(d, 5) = 10;	//会无法编译

虽然d[5]返回的是int&，但是auto会去掉引用修饰，则返回值类型就变成了int,作为函数的返回值，该int是一个右值，这在C++属于被禁止的行为。
```



进一步可以修改为：

```c++
template<typename Container, typename Index>
decltype(auto)
authAndAccess(Container & c, Index i)
{
    authenticateUser();
    return c[i];
}
```



`decltype`并不限于在函数返回值型别处使用。在变量声明的场合上，若你想用在初始化表达式处应用`decltype`型别推导：

```c++
Widget w;

const Widget & cw = w;

auto myWidget1 = cw;		//型别为 Widget, 复制构造函数

decltype(auto) myWidget2 = cw; //const Widget, 因为auto会在复制时忽略const, &, valotile, 但是decltype(auto)，不会，考虑之前的例子。
```



将`decltype`应用于一个名字之上，就会得出该名字的声明型别。名字其实是左值表达式，但是如果只有一个名字，`decltype`的行为保持不变。不过，如果是比仅有名字更复杂的表达式的话，`decltype`就保证得出的型别总是左值引用。



我们来看一个例子：

```c++
int x = 0;
//
其中x是一个变量名字，所以decltype(x)是一个int。;
但是如果把名字x放入一对小括号中，就得到了一个比仅有名字更复杂的表达式（x）.;
作为一个名字，x是一个左值，而在C++定义中，表达式（x）也是一个左值，所以decltype((x))的结果就成了int&.
```



###### 总结

- 绝大多数情况下，`decltype`会得出变量或表达式的型别而不作任何修改。
- 对于型别为T的左值表达式，除非该表达式仅有一个名字，`decltype`总是得出型别`T&`
- C++14支持`decltype(auto)`，和`auto`一样，它会从其初始化表达式出发来推导类型，但是它的型别推导使用的是`decltype`的规则。



# 条款4，掌握查看型别推导结果的方法

###### IDE编译器

IDE中的代码编辑器通常会在你将鼠标悬停在某个程序实体，显示出该实体的类别。

###### 运行时输出

```c++
int x = 0;
const int y = 3;

std::cout << typeid(x).name() << std::endl;
std::cout << typeid(y).name() << std::endl;

//输出
i
i
    
```



针对某个对象，如`x,y`调用`typeid`，就得到了一个`std::type_info`对象，而后者拥有一个成员函数`name`，该函数产生一个代表C-style的字符串。

对于`std::type_info::name`不保证返回任何有意义的内容。其中`i`表示`int`，而`PKi`表示`pointer to konst const`。



而这种方法并不一定准确，需要使用

```c++
#include <boost/type_index.hpp>

using boost::typeindex:;type_id_with_cvr;

cout << type_id_with_cvr<T>().pretty_name();

cout << type_id_with_cvr<decltype(param)>().pretty_name();


```

