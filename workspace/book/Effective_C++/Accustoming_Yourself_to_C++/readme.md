# 让自己习惯C++

## 视C++为一个语言联邦

今天的C++已经是一个多重范型编程语言（multiparadigm programming language）。支持：

- 过程形式（procedural）
- 面向对象形式（object-oriented）
- 函数形式（functional）
- 泛型形式（generic）
- 元编程形式（metaprogramming）



###### C++的四个次语言

- **C**。C++仍然是以C为基础。
- **Object-Oriented C++**。
- **Template C++**。这是C++泛型编程的部分。
- **STL**。C++模板程序库。



==C++高效编程守则视状况变化而变化，取决于你使用C++的哪一部分==



## 尽量以const，enum，inline，替换 #define

```c++
#define ASPECT_RATIO 1.653
```

该记号也许从未被编译器看见。也许在编译器开始处理源码之前就被预处理器移走了。

**使用的名称可能从未进入记号表**。

解决：

```c++
const double AspectRatio = 1.653; //大写名称常用于宏
```

作为一个语言常量，`AspectRatio`肯定会被编译器看到，当然就会进入记号表。



此外，对于浮点常量而言，使用常量可能比使用`#define`导致较小的码，因为预处理器**盲目地将宏名称`ASPECT_RATIO`**替换为1.653可能导致目标码出现多份1.653。



##### 定义常量指针（constant pointers）

由于常量定义式通常被放在头文件内（以便被不同地源码含入），因此有必要将指针（而不是指针所指之物）声明为`const`，如果要在头文件内定义一个常量的`char*-based`字符串，必须`const`**两次**：

```c++
const char * const authorName = "Scott Meyers";
```

而使用`string`比`char*-based`更好：

```c++
const std::string authorName = "Scott Meyers";
const std::string authorName("Scott Meyers");
```



##### class专属常量

为了将常量的作用域限制于`class`内，你必须让它成为`class`的一个成员；而为此确保此常量至多只有一份实体，你必须让他成为一个`static`成员：

```c++
class GamePlayer
{
    private:
    	static const int NumTurns = 5; // 常量声明式
    	int scores[NumTurns];          //使用该常量
    	...;
};
```

> 然而你所看到的`NumTurns`是**声明式**而非定义式。
>
> 定义式：
>
> ```c++
> const int GamePlayer::NumTurns;
> ```

注意：我们无法利用`#define`创建一个`class`专属常量，因为`#define`并不重视作用域。一旦被定义，就在其后的编译过程中有效（除非在某处被`#undef`）。



###### 上述方法的补偿做法 （the enum hack）

```c++
class GamePlayer
{
    private:
    	enum {NumTurns = 5;}
    	int scores[NumTurns];
};
```

因为取`enum`的地址是不合法的，所以如果你不想让别人获得一个`pointer`或`reference`指向你的某个整数常量，`enum`可以帮助你实现这个约束。



##### #define实现宏

宏看起来有点像函数，但是不会招致函数调用的额外开销

```c++
//以a和b的较大值调用f
#define CALL_WITH_MAX(a,b) f((a) > (b) ? (a) : (b))
```



必须为宏中的所以实参加上小括号，否则某人在表达式中调用这个宏可能会遇到麻烦。但是就算加了括号，还是会存在一些问题：

```c++
int a = 5;
int b = 0;
CALL_WITH_MAX(++a,b);			//a增加两次
CALL_WITH_MAX(++a,b + 10);		//a增加一次
```



`template inline`函数能够获得宏带来的效率以及一般函数所有可预料行为和类型安全性。

```c++
template<typename T>
inline void callWithMax(const T& a, const T& b)
{
    f(a > b ? a : b);
}
```



==对于单纯常量，最好以`const`对象或`enums`替换`#defines`==

==对于形似函数的宏（macros），最好用`inline`函数替换`#defines`==



## 尽可能使用const

`const`一件奇妙的事情是，它允许你指定一个语义约束（也就是一个**不该被改动的对象**），而编译器会强制实施这项约束。

```c++
char greeting[] = "hello"; 			//non-const pointer, non-const data;
char * p = greeting;				//non-const pointer, non-const data;

const char* p = greeting;			//non-const pointer, const data;
char* const p = greeting;			//const pointer, non-const data;
const char* const p = greeting;		//const pointer, const data;
```



> 如果`const`出现在星号左边，那么被指物是常量；如果出现在右边，那么指针是常量。如果出现在星号两边，那么指针和被指物都是常量。



##### `STL`迭代器

其以指针为根据塑膜出来，所以迭代器的作用就像一个`T*`指针。声明迭代器为`const`就像声明指针为`const`一样（即声明一个`T* const`指针），表示这个迭代器不得指向不同的东西，但它所指的值是可以改动的。

如果你希望迭代器所指的东西不可以被改动，需要一个`const_iterator`：

```c++
std::vetor<int> vec;
...;
const std::vector<int>::iterator iter = vec.begin(); //iter的作用像一个T * const
*iter = 10;				//没问题，改变所指之物
++iter;					//错误！iter是一个const


std::vector<int>::const_iterator cIter = vec.begin(); //cIter的作用像一个const * T
*cIter = 10;			//错误！*cIter是一个const
++cIter; 				//没问题
```



##### `const`函数

在一个函数声明式内，`const`可以和函数返回值、各参数、函数自身产生关联。

令函数返回一个常量值，往往可以降低因客户错误而造成的意外，而又不至于放弃安全性和高效性。

```c++
class Rational {...;};
const Rational operator* (const Rational & lhs, const Rational & rhs);
```



**为什么要返回const类型？**

观察如下代码：

```c++
Rational a , b, c;
...;
(a * b) = c; //实际上是(a * b) == c
```



如果`a,b`都是内置类型，上述代码直接就会不合法。

而一个良好的用户自定义类型的特征是避免无端地与内置类型不兼容，因为允许对两值乘积做赋值动作也没什么意义了。而声明为`const`可以预防这种行为。



##### `const`成员函数

将`const`实施于成员函数的目的，是为了确认该成员函数可作用于`const`对象身上。

- 它们使用`class`接口比较容易理解。得知那个函数能够改动对象内容而哪个函数不行。
- 它们使操作`const`对象成为可能。

考虑下面`class`:

```c++
class TextBlock
{
    public:
    	...;
    const char& operator[](std::size_t position) const {return text[position];}
    char& operator[](std::size_t position){return text[position];}
    
    private:
    	std::string text;
};

//main.cpp
TextBlock tb("Hello");
std::cout << tb[0];			//调用non-const

const TextBlock ctb("World");
std::cout << ctb[0];		//调用const

tb[0] = 'x';			//没问题。
ctb[0] = 'x';			//错误！，企图对一个 由const版之operator[]返回的const char&施行赋值
```



上述的例子比较造作，真实情况一般如下：

```c++
void print(const TextBlock & ctb)
{
    std::cout << ctb[0];		//调用const
}
```



> `non-const operator[]` 的返回类型是一个**reference to char**，不是char。如果是char。那么下面的句子就无法通过编译：
>
> ```c++
> tb[0] = 'x';
> ```
>
> 如果函数的返回类型本身是一个内置类型，那么改动函数的返回值本身就不合法。纵使合法，意味被改动的`tb.text[0]`是一个副本，而不是其本身。



##### bitwise constness

该学派的人认为，成员函数只有在不更改对象之任何成员变量（`static`除外）时才可以说时`const`。也就是不更改对象内的任何一个`bit`。

但是一个更改了**指针所指物**的成员函数虽然不能算是`const`，但是如果只有指针隶属于对象，那么我们称此函数为`bitwise const`不会引发编译器异议。

```c++
class CTextBlock
{
    public:
    	char & operator[](std::size_t position) const {
            return pText[position]; //但其实不适当。
        }//bitwise const 声明
    private:
    	char * pText;
};
```

这个`class`不适当地将器`operator[]`声明为`const`成员函数，而该函数却返回一个`reference`指向对象内部值。

但是我们看看它允许发生什么事：

```c++
const CTextBlock cctb("Hello");			//声明一个常量对象
char * pc = &cctb[0];					//调用const operator[]取得一个指针

*pc = 'J';								//cctb变成Jhello这样的内容
```

最后还是改动了其值。

##### logical constness

一个`const`成员函数可以修改它所处理的对象内的某些`bits`，但只有在客户端侦测不出来的情况下才得如此。

假如你的`CTextBlock`有可能高速缓存文本区块的长度以便应付询问：

```c++
class CTextBlock
{
    public:
    	...;
    	std::size_t length() const;
    private:
    	char * pText;
    	std::size_t textLength;			//最近一次计算的文本区块长度
    	bool lengthIsValid;				//目前的长度是否有效
};

std::size_t CTextBlock::length() const
{
    if(!lengthIsValid){
        textLength = std::strlen(pText);
        lengthIsValid = true;
    }//错误！不能在const函数里面给两个变量赋值
    return textLength;
}
```



为了能够修改，利用C++一个与`const`相关的摆动场：`mutable`（可变的）。

`mutable`释放掉`non-static`成员变量的`bitwise constness`约束：

```c++
class CTextBlock
{
    public:
    	...;
    	std::size_t length() const;
    private:
    	char * pText;
    	mutable std::size_t textLength;			//最近一次计算的文本区块长度
    	mutable bool lengthIsValid;				//目前的长度是否有效
};

std::size_t CTextBlock::length() const
{
    if(!lengthIsValid){
        textLength = std::strlen(pText);
        lengthIsValid = true;
    }//现在可以修改了
    return textLength;
}
```



##### 在`const`和`non-const`成员函数中避免重复

`mutable`是一个解决办法，但是不能解决所有`const`相关难题。如果在原来的函数内容中增加一些东西

```c++
class TextBlock
{
    public:
    	const char& operator[](std::size_t position) const
        {
            ...;//边界检验（bounds checking）
            ...;//标志数据访问（log access data）
            ...;//检验数据完整性（verify data integrity）
            return text[position];
        }
    	char & operator[](std::size_t position)
        {
            ...;
            ...;
            ...;
            return text[position];
        }
    private:
    	std::string text;
};
```

里面会涉及到一些重复的代码。

所以真正应该做的是实现`operator[]`的机能一次并使用它两次。也就是说，必须令其中一个调用另一个。

这促使我们将常量性转除（casting away constness）

```c++
class TextBlock
{
  	public:
    	...;
    	const char & operator[](std::size_t position) const
        {
            ...;
            ...;
            ...;
            return text[position];
        }
    	char & operator[](std::size_t position) //只调用const op[]
        {
            return 
                const_cast<char&>(		//将op[]返回值的const转除
            		static_cast<const TextBlock&>(*this) //为*this加上const
            			[positon]						//调用const op[]
            );
        }
};
```

> 这份代码有两个转型动作。目的是让`non-const`调用`const`，但是`non-const operator[]`内部调用若只是单纯调用会调用自己。为了避免无穷递归，我们必须明确指出`const operator[]`，但是C++缺乏直接的语法可以那么做。
>
> 步骤：
>
> 1. `*this`类型从其原始类型`TextBlock&`转型为`const TextBlock&`，这是一次强迫安全型转型（将`non-const`对象转型为`const`对象），使用`static_cast<>`。
> 2. 从`const operator[]`的返回值中移除`const`。



==将某些东西声明为`const`可以保证编译器侦测出错误用法。`const`可以被施加于任何作用域内的对象、函数参数、函数返回类型、成员函数本体==

==编译器强制实施`bitwise constness`，但是编写程序时应该使用概念上的常量性==

==当`const`和`non-const`成员函数有着实质性等价的实现时，令`non-const`版本调用`const`版本可以避免代码重复==



## 确定对象被使用前已经被初始化

读取未初始化的值会导致不明确的行为。

对于无任何成员的内置类型，你必须手工完成此事。

```c++
int x = 0;
const char * test = "A C-style string";

duoble d;
std::cin >> d;			//以读取input stream 的方式完成初始化
```



至于内置类型之外的东西，**确保每一个构造函数都将对象的每一个成员初始化**。

但是不要混淆赋值和初始化。我们考虑下面这个通讯簿的类

```c++
class PhoneNumber {...;};
class ABEntry{ 			//ABEntry = "Address Book Entry"
    public:
    	ABEntry(const std::string & name, const std::string & address,
                const std::list<PhoneNumber> & phones);
    private:
    	std::string theName;
    	std::string theAddress;
    	std::list<PhoneNumber> thePhones;
    	int numTimesConsulted;
};

ABEntry::ABEntry(const std::string & name, const std::string & address,
                 const std::list<PhoneNumber> & phones)
{
    theName = name;
    theAddress = address;
    thePhones = phones;
    numTimesConsulted = 0;
    //以上都是赋值，而不是初始化
}
```



**C++规定，对象的成员变量的初始化动作发生在进入构造体本体之前。**

但是这对`numTimeConsulted`不为真，因为它属于内置类型，不保证一定在你所看到的哪个赋值动作时间点之前获得初值。



构造函数的一个较佳写法是，使用`member inittialization list`替换赋值：

```c++
ABEntry::ABEntry(const std::string & name, const std::string address,
                 const std::list<PhoneNumber> & phones)
    : theName(name),
	  theAddress(address),
	  thePhones(phones),
	  numTimesConsulted(0)
          //以上这些都是初始化
{}
```



两者的结果相同，但是后者的效率更高。



第一个版本先调用构造函数初始化，然后再赋予新值。



==总是使用成员初值列==。



###### C++的固定初始化次序

1. 基类比派生类更早初始化。
2. 类的成员变量按照声明的次序进行初始化。



###### 不同编译单元内定义之`non-local static`对象的初始化次序

所谓`static`对象，其寿命从被构造出来直到程序结束为止，因此`stack`和`heap-based`对象都被排除。



现在，我们关心的问题涉及至少两个源码文件，每一个内含至少一个`non-local static`对象（也就是说该对象是`global`或位于`namespace`作用域内，抑或在`class`内或`file`作用域内被声明为`static`）。

真正的问题是：如果某编译单元内的某个`non-local static`对象的初始化动作使用了另一编译单元内的某个`non-local static`对象，它所用到的这个对象可能尚未被初始化，因为C++没有明确定义其初始化次序。



我们来看下面的例子：

```c++
class FileSystem		//来自你的程序库
{
    public:
    	...;
    	std::size_t numDisks() const; //众多成员函数之一
    	...;
};

extern FileSystem tfs;		//预留给客户使用的对象
```



现在假设某些客户建立了一个`class`用于处理文件系统内的目录，那么它们用上`tfs`对象。

```c++
class Directory		//由程序库客户建立
{
    public:	
    	Directory(params);
    	...;
};

Directory::Directory(params)
{
    ...;	
    std::size_t disks = tfs.numDisks();
    ...;
}

Directory tempDir(params)
```



那么如何确定`tfs`在`tempDir`之前初始化？



C++之所以没有定义`non-local static`的初始化次序，是因为：

**决定它们的初始化次序相当困难，也就是多个编译单元内的`non-local static`对象经由模板隐式具现化形成**。



幸运的是，需要做的就是**将每个`non-local static`对象搬到自己的专属函数内（该对象在此函数内被声明为`static`）这些函数返回一个`reference`指向它所含的对象，然后用户调用这些函数，而不是直接调用这些对象**。

换句话说，`non-local static`被`local static`对象替换了，这就是单例（singleton）。

这个手法的基础在于，C++保证，函数内的`local static`对象会在**该函数被调用期间，首次遇上该对象之定义式**时被初始化。

所以如果你以函数调用替换直接访问`non-local static`对象，就得到了一个保证被初始化的对象。

具体如下：

```c++
class FileSystem {...;};		//同前
FileSystem & tfs()		//用这个函数替换tfs对象
{
    static FileSystem fs;			//定义并初始化一个local static对象
    return fs;						//返回一个引用
}
class Directory {...;};
Directory::Directory(params)
{
    ...;
    std::size_t disks = tfs().numDisks();
    ...;
}

Directory & tempDir()
{
    static Directory td;
    return td;
}

```



==为内置型对象进行手工初始化，因为C++不保证初始化它们==

==构造函数最好使用成员初值列，而不要在构造函数本体内使用赋值操作。初值列列出的成员变量，其排列次序应该和它们在`class`中的声明次序相同==

==为免除 跨编译单元之初始化次序问题，请以`local static`对象替换`non local static`对象==

