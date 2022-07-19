# 设计与声明

## 让接口容易被正确使用，不易被误用

**考虑用户做出什么样的错误**

假设设计一个用来表示日期的类：

```c++
class Date
{
    public:
    	Data(int month, int day, int year);
    	...;
};

//这个接口看起来可能没什么问题，但是用户可能传递的是
Date d(30,3,1995); //应该是3，30
Date d(2,30,1995); //用户打错，是3,30
```



许多客户端错误可以因为导入新类型而获得预防。我们可以导入简单的外覆类型来区别天数、月份和年份

```c++
struct Day{
    explicit Day(int d) : val(d) {}
    int val;
};

struct Month{
    explicit Month(int m) : val(m) {}
    int val;
};

struct Year{
    explicit Year(int y) : val(y) {}
    int val;
};

class Date{
    public:
    	Date(const Month & m, const Day & d, const Year & y);
    	...;
};

Date(30,3,1995);						//错误，不正确的类型
Date(Month(30),Month(3),Year(1995));	//错误，不正确的类型
Date(Month(3),Day(30),Year(1995));		//类型正确
```



一旦正确的类型就定位，限制其值有时候是同情达理的。假如一年只有12个月，所以Month应该反映这一事实。

我们可以使用`enum`，但是其安全性不够。

```c++
class Month
{
    public:
    	static Month Jan() {return Month(1);}
   		static Month Feb() {return Month(2);}
    	...;   	
    	static Month Dec() {return Month(12);}
    	...; 			//other
    private:
    	explicit Month(int m);		//阻止生成新的月份；
    	...;						//月份专属数据 
};

Date d(Month::Mar(), Day(30),Year(1995));
```



预防客户错误的另一个方法是，限制类型什么事可以做，什么事不能做。比较常用的方法是加上`const`。



任何接口如果要求客户必须记得做某些事情，就是有着**不正确使用**的倾向，因为客户可能会忘记做那件事。



==好的接口很容易被正确使用，不容易被误用。你应该在你的所有接口中努力达成这些性质==

==促进正确使用   的办法包括接口的一致性，以及内置类型的行为兼容==

==阻止误用  的办法包括建立新类型、限制类型上的操作，束缚对象值，以及消除客户的资源管理责任==

==tr1::shared_ptr支持定制型删除器。这可以防范DLL问题，可被用来自动解除互斥锁==



## 设计CLASS犹如设计TYPE

当你定义了一个`class`，也就是定义了一个`type`。

如何设计高效的`class`，请回答下面的问题：

- **新type的对象应该如何被创建和销毁？**这会影响到你的`class`的构造函数和析构函数以及内存分配函数和释放函数（`operatornew, operatornew[],operator delete,operator[]`）的设计。
- **对象的初始化和对象的赋值该有什么差别？**这个答案决定了你构造函数和赋值操作符的行为，以及其间的差异。很重要的是别混淆了**初始化**和**赋值**，因为它们对应于不同的函数调用。
- **新type的对象如果被passed by valued，意味着什么？**记住，copy构造函数用来定义一个`type`的`pass-by-value`该如何实现？
- **什么是新type的合法值？**对`class`的成员变量而言，通常只有某些数值是有效的。那些数值集决定了你的`class`必须维护的约束条件。也就决定了你的成员函数必须进行的错误检查工作。它也影响函数抛出的异常、以及函数异常明细列。
- **你的新type需要配合某个继承图系吗？**如果你继承自某些既有的`classes`，你就受到那些`classes`的设计的束缚，特别是受到虚函数和非虚函数的影响。如果你允许其他类继承你的类，那会影响你声明函数——尤其是析构函数，是否为`virtual`。
- **你的新type需要什么样的转换？**你的type生存于其他海票type之间，因此彼此应该又转换行为吗？如果你希望允许类型T1之物被隐式转换T2之物，就必须在T1内写一个类型转换函数或在T2内写一个构造函数。
- **什么样的操作符和函数对此新type而言是合理的？**其中某些该是成员函数，其中某些应该不是。
- **什么样的标准函数应该驳回？**声明为`private`。
- **谁该取用新type的成员？**思考什么为`public,private,protected`以及是否为友元。
- **什么是新type的未声明接口？**它对效率、异常安全性以及资源运用提供何种保证？你在这些方面提供的保证将为你的类实现代码上加上相应的约束条件。
- **你的新type有多么一般化？**模板类。
- **你真的需要一个新type吗？**如果只是定义新的继承类以便为既有的类添加机能，那么说不定单纯定义一个或多个非成员函数和模板更能达到目标。

## 宁以pass-by-reference-to-const替换pass-by-value

缺省情况下C++以pass by value方式传递对象至函数。传递的是一个副本。

参考以下一个例子：

```c++
class Person
{
    public:
        Person();
        virtual ~Person();
    private:
        std::string name;
        std::string address;
};


class Student : public Person
{
    public:
        Student();
        ~Student();
    private:
        std::string schoolName;
        std::string schoolAddress;
};

//参考以下代码
bool validateStudent(Student s);
Student plato;
bool platoIsOK = validateStudent(plato);
```



当上述函数被调用时，其中`Student`的构造函数会被调用，以`plato`为蓝本初始化`s`。同样，当`validateStudent`结束时会销毁`s`，会调用一次析构函数。

但是，我们要注意的是，类`Student`中还有两个`string`对象，然后`Student`是继承自`Person`。除了调用两个`string`的构造函数外，还要调用`Person`的构造函数。`Person`中还有两个`string`。。。。。

而使用

```c++
bool validateStudent(const Student & s);		//pass-by-reference-to-const
```



这种方式不涉及任何构造函数和析构函数的调用，因为没有任何对象被创建。其中`const`能够防止其被修改。



以by-reference方式传入参数能够避免**对象切割**问题。当一个派生类对象以by value方式并被视为一个基类对象，基类的构造函数被调用，而造成此对象的行为像个派生类对象的那些特化性质被切割掉了。我们可以看下面的例子：

```c++
class Window
{
    public:
    	...;
    	std::string name() const;
    	virtual void display() const;
    
};

class WindowWithScrollBars : public Window
{
    public:
    	...;
    	virtual void display() const;
};


//我们来看一个错误例子
void printNameAndDisplay(Window w)
{
    std::cout << w.name();
    w.display();
}//当我们给这个函数传递一个WindowWithScrollBars，会发生什么？

因为参数传递的是Window对象，所以在函数里面调用时使用的是Window::name();

所以为了解决切割问题，我们需要传递引用
void printNameAndDisplay(const Window & w)
{
    std::cout << w.name();
    w.display();
}
```



内置类型往往采用pass by value效率更高。



==尽量以pass-by-reference-const替换pass-by-value。前者通常比较高校，并可避免切割问题。==

==以上规则并不适用于内置类型，以及STL的迭代器和函数对象。对它们而言，pass-by-value往往比较适当。==



## 必须返回对象时，别妄想返回其reference

我们希望在返回值时避免构造函数和析构函数的调用。

我们来看下面一个例子：

```c++
class Rational
{
    public:
    	Rational(int numberator = 0, int denominator = 1);
    	...;
    private:
    	int n, d;		//分子和分母
    	friend const Rational operator*(const Rational & lhs, const Rational& rhs);
    	//这个const是因为防止必要的判断==  变成赋值符=，客户使用的意外。
};
```



**注意，当你在任何时候看到reference时，都要思考它的另一个名称是什么？**



那么

```c++
Rational a(1,2);			//1/2
Rational b(3,5);			//3/5
Rational c = a * b;			//3/10
//因为重载了，所以如果当我们把这段的返回加上reference，那么就期望存在一个3/10的对象。所以需要自己船舰一个对象让reference指向那个对象，但是我们不能在函数内部创建一个对象，然后返回让一个referce，这样函数结束时，被指向的对象会被销毁。同样的道理也适用于指针
比如下面这个错误例子
const Rational & operator*(cosnt Rational & lhs, const Rational & rhs)
{
    Rational result(lhs.n * rhs.n, lhs.d * rhs.d);
    return result	//非常糟糕的操作。
}

//那我们尝试在heap中创建一个对象，
const Rational & operator*(const Rational & lhs, const Rational & rhs)
{
    Rational * result = new Rational(lhs.n * rhs.n, lhs.d * rhs.d);		//调用了构造函数
    return *result;		//谁来delete？
}

w = x * y * z;		//这里会调用两次new，但是有一次new没有办法delete，一定会内存泄露。
```



那让我们试试`static`？

```c++
const Rational & operator*(const Rational & lhs, const Rational & rhs)
{
    static Rational result;
    
    result = ...;
    
    return result;
}

//如果我们尝试使用下面的代码
bool operator==(const Rational & lhs, const Rational & rhs)
{
}

Rational a, b, c, d;
...;
if((a * b) == (c * d)) {}		//一定会为true，因为返回的是同一个内存。
else {}


这里有一个问题，因为我们知道引用是不能更改指向的对象的，因为一旦更改指向的对象，其实是把原来对象的值给改成了另一个对象的值。这里虽然static中的值改变了两次，但是由于是引用的，最后其值都会相同。
```



所以如果要返回一个新对象，那么就必须返回一个新对象，但是必须接收调用构造函数的成本。可以使用`inline`来消除函数调用。



==绝不要返回pointer和reference指向一个local  stack对象，或返回reference指向一个heap-allocated对象，或返回pointer或reference指向一个local static对象而有可能同时需要多个这样对象。考虑单例==



## 将成员变量声明为private

###### 为什么不采用`public`?

- 如果成员变量不是`public`，那么在访问成员变量的时候就只能使用成员函数。那么所有的都是统一的。这是说明了类的一致性。

- 使用成员函数能够对成员变量有更精准的控制，比如出现：不准访问、只读访问、读写访问，或者是唯写访问。

- 封装性。



成员变量的封装性与成员变量的内容改变时所破坏的代码数量成反比。

从封装的角度看，其实只有两种访问权限，封装与不封装。

==切记将成员变量声明为private。这可赋予客户访问数据的一致性，可细微划分访问控制，允诺约束条件获得保证，并提供class作者充分的弹性==

==protected并不比public，更具封装性。==

## 宁以non-member、non-friend替换member函数。

我们看一个例子先：

```c++
class WebBrowser
{
    public:
    	...;
    	void ClearCache();			//高速缓存区
    	void ClearHistory();		//清除历史记录
    	void RemoveCookies();		//清除cookies
    	...;
    	//可能会一起执行这个动作
    	void ClearEverything();		//同时执行上述三个动作
    	...;
};

//我们可以用一个非成员函数来实现
void ClearBrowser(WebBrowser & wb)
{
    wb.ClearCache();
    wb.ClaerHistory();
    wb.RemoveCookies();
}
```



###### 那么上述的成员函数更好，还是非成员函数更好呢？

面向对象守则要求，数据以及操作数据的那些函数应该被捆绑在一块，也就说成员函数是更好的选择。但是这个建议不正确，这是面向对象基于**真实意义的一个误解。**

实际上，在这个例子中，成员函数的封装性低于非成员函数。因为非成员非友元函数并不增加能够访问类内私有成分的函数数量。

此外，提供非成员函数可以允许对该类相关机能有较大的包裹弹性，最终导致较低的编译相关度，增加该类的可延展性。



对于上述的例子，一个比较好的做法是让`ClearBrowser`函数成为一个非成员函数并位于`WbeBrowser`所在的同一个命名空间。

```c++
namespace WebBrowserStuff
{
    class WebBrowser{};
    void ClearBrowser(WebBrowser & wb);
}


//我们可以采用如下的方式分离一些功能
//头文件webbrowser.h，针对该类本身
namespace WebBrowserStuff
{
    class Browser {};
    //核心功能，所有用户都需要的。
    //非成员函数
}

//webbrowserbookmarks.h
namespace WbrBrowserStuff
{
    ...;//与书签相关的遍历函数
}

//webbrowsercookies.h
namespace WebBrowserStuff
{
    ...;//与cookie相关的便利函数
}

```



==宁可拿非成员非友元函数替换成员函数。这样做可以增加封装性、包裹弹性和机能扩展性。==



## 若所有类型皆需类型转换，请为此采用非成员函数

假设你设计一个类来表现有理数，允许整数隐式转换。

```c++
class Rational
{
    public:
    	Rational(int numberator = 0, int denominator = 1);
    	int numerator() const;
    	int denominator() const;
    	const Rational operator*(const Rational & rhs) const;
    private:
    	...;
};


//上述类的设计调用如下：
Rational oneEighth(1,8);
Rational oneHalf(1,2);
Rational result = oneHalf * oneEight;			//OK
result = result * oneEighth;					//OK


//可能还需要一些混合运算
result = oneHalf * 2;			//OK
result = 2 * oneHalf;			//WRONG

//我们修改上述代码的编写方式
result = oneHalf.operator*(2);			//隐式转换
result = 2.operator*(oneHalf);			//明显错误
//这里要注意，因为上述没有，编译器就会试图寻找非成员函数
result = operator*(2,oneHalf);			//没有写这个函数
//如果加上了explicit Rational， 上述的两段代码都错误。

```



那么我们尝试使用非成员函数：

```c++
class Rational
{
  	...;//和上述一样，但是不包括operator*  
};

const Rational operator*(const Rational * lhs, const Rational & rhs)
{
    return Rational(lhs.numerator() * rhs.numerator(),lhs.denominator() * rhs.denominator());
}

//以下都OK
result = oneHalf * 2;
result = 2 * oneHalf;
```



那么是否需要将起变成友元函数呢？

但是是不需要，因为完全可以通过接口完成，不需要访问私有成员变量。



==如果你需要为某个函数的所有参数（包括被this指针所指的那个隐喻参数）进行类型转换，那么这个函数必须是非成员函数==



## 考虑写出一个不抛出异常的swap函数

`swap`是一个有趣的函数，原本它只是STL的一部分，而后成为异常安全性编程的脊柱，以及用来处理自我赋值可能性的一个常见机制。

`swap`就是交换两对象值。

```c++
namespace std
{
    template<typename T>
    void swap(T & a, T & b)
    {
        T temp(a);
        a = b;
        b = temp;
    }
}
```



但是对于某些类型而言，这些复制动作无一必要。

其中最主要的就是**以指针指向一个对象，内含真正数据**的那种类型。被称pimpl（pointer to implementation）手法。

```c++
class WidgetImpl
{
    public:
    	...;
    private:
    	int a, b, c;				//可能有很多数据
    	std::vector<double> v;		//因为复制很长时间
    	...;
};


class Widget			//pimpl
{
    public:
		Widget(const Widget & rhs);
    	Widget & operator=(const Widget & rhs)
        {
            ...;
            *pImpl = (*rhs.pImpl);
            ...;
		}
    	...;
    private:
    	WidgetImpl * pImpl;
};
```



一旦要换两个对象，其实只需要换两个指针就行了。但是swap不知道，要把所有的对象都复制一边。

我们可以特化swap函数

```c++
namespace std
{
    template<>
    void swap<Widget>(Widget & a, Widget & b) //这是std::swap针对T是Widget的特化版本。
    {
        swap(a.PImpl, b.Pimpl);				//私有变量，还不能通过编译。
    }
}

//我们可以为其声明一个友元版本
void swap(Widget & other)
{
    using std::swap;
    swap(pImpl,other.pImpl);
}

//修改后
namespace std
{
    template<>
    void swap<Widget>(Widget & a, Widget & b) //这是std::swap针对T是Widget的特化版本。
    {
        a.swap(b.pImpl);			//私有变量，还不能通过编译。
    }
}


```



==当std::swap对你的类型效率不高时，提供一个swap成员函数，并确定一个函数不抛出异常==

==如果你提供一个成员swap，也该提供一个非成员swap来调用前者。对于类，也请特化swap==

==调用swap时应针对std::swap使用using声明式，然后调用swap并且不带任何命名空间修饰==

==千万不要在std空间加入一些对std来说是新的东西==



