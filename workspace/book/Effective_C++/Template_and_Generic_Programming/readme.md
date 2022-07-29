# 模板与泛型编程

## 了解隐式接口和编译期多态

面向对象世界总是以显示接口和运行期多态解决问题，比如：

```c++
class Widget
{
    public:
    	Widget();
    	virtual ~Widget();
    	virtual std::size_t size() const;
    	virtual void normalize();
    	void swap(Widget & other);
};

void doProcessing(Widget & w)
{
    if(w.size() > 0 && w != someNastyWidget)
    {
        Widget temp(w);
        temp.normalize();
        temp.swap(w);
    }
}
```



于是，我们对于函数中的w：

- 由于w的类型被声明为`Widget`，所有w必须支持`Widget`接口。我们可以在源码中找到这个接口，其明确可见，所以我们称其为显式接口。
- 由于`Widget`的某些成员是`virtual`，也就是会表现出运行时多态。



但是如果我们将其转换为函数模板，其隐式接口和编译期多态就更重要了：

```c++
template<typename T>
void doProcessing(T & w)
{
    if(w.size() > 10 && w != someNastyWidget)//隐式接口，必须有一个名为size的函数返回整数值，必须支持operator!=来比较两个T对象。
    {
        T temp(w);
        temp.normalize();
        temp.swap(w);
	}
}
```



那么对于现在的w：

- w必须支持哪一种接口，由template中执行于w身上的操作来决定。（对此template而言必须有效编译）这一组表达式便是T必须支持的一组隐式接口。
- 凡涉及w的任何函数调用，有可能造成模板的具现化，使这些调用成功。这些具现化的行为发生在编译器。**以不同的模板参数具现化会导致调用不同的函数**，这就是编译期多态。



编译期多态：哪个重载函数该被调用。

运行期多态：哪一个`virtual`函数将被绑定。



通常来说，显示接口由函数的签名式组成。隐式接口由有效表达式组成。



==类和模板都是支持接口和多态==

==对类而言接口是显式的，以函数签名为中心。多态则是通过virtual函数发生于运行期==

==对模板参数而言，接口时隐式的，奠基于有效表达式。多态则是通过模板具现化和函数重载解析发生于编译期==



## 了解typename的双重意义

来看一个问题，`typename`和`class`有什么不同？

```c++
template<class T> class Widget;
template<typename T> class Widget;
```



> 没有不同。两者意义完全相同。但是这并不意味着`class`和`typename`等价。

来个一个没有实际意义的例子：

```c++
template<typename T>
void print2nd(const C& container)
{
    if(container.size() >= 2)
    {
        C::const_iterator iter(container.begin());	//基于规则，const_iterator会被认为呈一个变量。
        ++iter;
        int value = *iter;
        std::cout << value;
    }
}

//来看一个例子
//如何造成解析困难？
template<typename T>
void print2nd(const C & container)
{
    c::const_iterator * x;
}
```



`iter`的类型是`const_iterator`，实际是什么取决于模板参数C。模板内出现的名称如果相依于某个模板参数，称之为从属名称。如果从属名称在类内呈嵌套状，则称为嵌套从属名称。

`value`类型是`int`。则被称为非从属名称。

上述代码块中的第二段代码，可能会造成解析困难，因为编译期实现并不知道其是否为一个类型还是一个变量。所以就无法得知是声明指针还是进行相乘操作。在编译器开始解析之前，还不知道C是什么。



但是C++有个规则：如果解析器在模板中遇到一个嵌套从属名称，便假设这个名称不是一个类型，除非你告诉他是。



**我们可以通过添加关键字typename来告诉编译器这是一个类型**，所以上述代码的修改如下：

```c++
template<typename T>
void pring2nd(const C& container)
{
    if(container.size() >= 2)
    {
        typename C::const_iterator iter(container.begin());
    }
}
```



一般性规则：**任何时候当你想要在模板中指涉一个嵌套从属名称，就必须在其紧临的前一个位置加上typename**，但是也有例外。而其他名称则不能有typename。



比如：

```c++
template<typename T>
void f(const C& container			//不允许使用typename,只是从属名称
       typename C::const_iterator iter);	//必须使用typename,嵌套从属名称
```



而上述规则的例外是：**typename不可以出现在base classes list内的嵌套从属名称之前，也不可在成员初值列作为基类修饰符**

```c++
template<typename T>
class Derived : public Base<T>::Nested		//base classes list，不允许typename
{
  	  public:
    	explicit Derived(int x)
            : Base<T>::Nested(x)			//初值列，不能加
            {
                typename Base<T>::Nested temp;	//这里要加，不是其他两个地方。
            }
};
```



==声明模板参数时，前缀关键字class和typename可以互换==

==请使用关键字typename标识嵌套从属类型名称，但不得在基类列或者是成员初值列内以它作为基类修饰符==



## 学习处理模板化基类内的名称

假设我们要写一个程序，它能够传送信息到若干个不同的公司去。如果编译期间我们有足够的信息来决定哪个信息传递至哪家公司，就可以采用模板的写法：

```c++
class CompanyA
{
  	public:
    	void sendCleartext(const std::string & msg);
    	void sendEncrypted(const std::string & msg);
    
};

class CompanyB
{
    public:
    	void sendCleartext(const std::string & msg);
    	void sendEncrypted(const std::string & msg);
};

class MsgInfo {};

template<typename Company>
class MsgSender
{
    void SendClear(const MsgInfo & info)
    {
        std::string msg;
        //产生信息
        Company c;
        c.sendCleartext(msg);
    }
    void SendSecret(const MsgInfo & info) {}	//相同，传输加密
};

template<typename Company>
class LoggingMsgSender : public MsgSender<Company>
{
    public:
    void sendClearMsg(const MsgInfo & info)
    {
        //记录
        sendClear(info);		//但是无法通过编译，编译器无法知道这个函数
        //记录
    }
};

```



因为继承的`MsgSender<Company>`，如果不知道`Company`是什么，那就没办法知道其中是否有一个`SendClear`函数。

为了让问题更具体化，假设我们有个公司只提供加密通信

```c++
class CompanyZ
{
    public:
        void sendEncrypted(const std::string * msg);
};

//但是一般的版本不支持，我们可以提供一个特化版本
template<>      //特化版声明
class MsgSender<CompanyZ> //全特化，没有别的参数可供变化。
{
    public:
        void SendSecret(const MsgInfo & info) {}
};

```



那么我们此时再来分析之前那段报错的代码就可以看出，如果`company`是`CompanyZ`，其没有`SendClear`函数（特化）。所以这就是为什么C++拒绝调用的原因，它知道其可能被特化，而那个特化版本可能不提供和一般性接口模板相同的接口。因为它往往拒绝模板化基类中寻找继承而来的名称。



那么如何解决该问题？

###### 在基类函数调用动作上加入`this`

```c++
template<typename Company>
class LoggingMsgSender : public MsgSender<Company>
{
    public:
    void sendClearMsg(const MsgInfo & info)
    {
        //记录
        this->SendClear(info);		
        //记录
    }
};
```



###### 使用using声明式

```c++
template<typename Company>
class LoggingMsgSender : public MsgSender<Company>
{
    public:
    using MsgSender<Company>::SendClear;    //告诉编译器假设SendClear位于基类内
    void sendClearMsg(const MsgInfo & info)
    {
        //记录
        SendClear(info);		
        //记录
    }
};
```



###### 明确指出

```c++
template<typename Company>
class LoggingMsgSender : public MsgSender<Company>
{
    public:
    void sendClearMsg(const MsgInfo & info)
    {
        //记录
        MsgSender<Company>::SendClear(info);		//明确说明
        //记录
    }
};
```



但是如果尝试使用如下代码，还是会发生报错：

```c++
LoggingMsgSender<CompanyZ> zMsgSender;
MsgInfo msgData;
zMsgSender.sendClearMsg(msgData);
```



==可在继承类模板内通过this->指涉基类模板内的成员名称，或借由一个明白写出基类资格修饰符完成。==



## 将与参数无关的代码抽离模板

来看一个例子关于模板中的代码重复：(固定尺寸的正方矩形，求逆)

```c++
template<typename T, std::size_t n>
class SquareMatrix
{
    public:
        void invert() {}            //求逆矩阵
};


SquareMatrix<double,5> sm1;
sm1.invert();
SquareMatrix<double,10> sm2;
sm2.invert();					//将会引发代码膨胀，因为除了5和10，其他没什么不同。
```



来看一个合理的修改

```c++
template<typename T>
class SquareMatrixBase
{
    protected:
    	SquareMatrixBase(std::size_t n, T * pMem)
            :size(n),pData(pMem) {}
    	void setDataPtr(T * ptr) {pData = ptr;}
    	
   	private:
    	std::size_t size;
    	T * pData;
};

template<typename T, std::size_t n>
class SquareMatrix : public SquareMatrixBase<T>
{
  	  public:
    	SquareMatrix()
            : SquareMatrixBase<T>(n,data) {}
    private:
    	T data[n * n];
};
```



==模板生成多个类和多个函数，所以任何模板代码都不该与某个造成膨胀的模板参数产生联系==

==因非类型模板参数而造成的代码膨胀，往往可以消除，做法是以函数参数或类成员变量替换模板参数==

==因类型参数而造成的代码膨胀，往往可减低。做法是让带有完全相同二进制表述的具现类型共享实现码==

## 运用成员函数模板接受所有兼容类型

所谓智能指针是行为像指针的对象，并提供指针没有的机能。真实指针做的一件比较好的事是支持隐式转换。

```c++
class Top {};
class Middle : public Top {};
class Bottom : public Middle {};
Top * pt1 = new Middle;
Top * pt2 = new Bottom;
const Top * pct1 = pt1;

//如果我们想自己编写指针来模拟上述转换
template<typename T>
class SmartPtr
{
    public:
    	explicit SmartPtr(T * realPtr);
};

SmartPtr<Top> pt1 = SmartPtr<Middle>(new Middle);
```



###### 模板和泛型编程

 接下来考虑编写智能指针的构造函数。但是为了满足所有类型的转换，可能需要编写很多构造函数。所以我们可以编写一个模板成员函数（成员函数模板）。

```c++
template<typename T>
class SmartPtr
{
    public:
    	template<typename U>
    	SmartPtr(const SmartPtr<U>& other);			//成员模板，构造函数
};
```



以上代码的含义是，对任何类型T和任何类型U，这里可以根据`SmartPtr<U>`生成一个`SmartPtr<T>`。是同一个模板的不同具现体，称之为泛化构造函数。不需要声明为`explicit`，是为了允许隐式转换。

但是我们需要进行一些限制，比如禁止完全不同类型之间的转换，以及禁止从派生类指针转向基类指针。

```c++
template<typename T>
class SmartPtr
{
  	public:
    	template<typename U>
    	SmartPtr(const SmartPtr<U>& other)
    	: heldPtr(other.get()) {...;}	//这里只有能通过隐式转换的才能通过编译。
    	T* get() const {return heldPtr;}
    private:
    	T* heldPtr;			//原始指针
};
```



泛化构造函数并不会组织编译器自己生成一个原始的构造函数。



==请使用成员函数模板 生成 可接受所有兼容类型的函数==

==如果你声明成员模板用于泛化copy构造或者是泛化赋值构造函数，还是需要声明正常的复制构造函数和赋值构造函数==



## 需要类型转换时请为模板定义非成员函数

继续之前的一个需要转换类型的一个例子，现在将它模板化

```c++
template<typename T>
class Rational
{
  	public:
    	Rational(const T& numberator = 0, const T& denominator = 1);
    	const T& numerator() const;
    	const T& denominator() const;
};


template<typename T>
const Rational<T>::operator*(const Rational<T> & lhe, const Rational<T>& rhs) {...;}


//与之前的例子相同，之前的例子可以通过编译
Rational<int> oneHalf(1,2);
Rational<int> result = oneHalf * 2;			//这里是错误的。

//这里需要修改operator为友元函数即可通过编译
template<typename T>
class Rational
{
  	public:
    	Rational(const T& numberator = 0, const T& denominator = 1);
    	const T& numerator() const;
    	const T& denominator() const;
    	friend Rational operator*(const Rational& lhs, const Rational & rhs);
};
//这是一种混合式调用方法，因为这个个函数在类里面，当模板具现化Rational<int>的时候，operator*也被创建出来了。
原因就在于，如果不是友元函数，那么两个参数可以是不一样的T，但是如果现在是友元函数，第一个T推导出来是int，那么第二个参数一定是Rational<int>，这时候就可以隐式转换int为Rational<int>。而第一种情况是无法将int隐式转换为Rational<T>的;

```



但是上述代码仅仅只能通过编译，而无法连接。这是因为没有定义上述的友元函数，只是在类中声明了友元函数。外部的函数是不算友元函数的定义的，因为不是相同的函数。



那么进行如下修改

```c++
template<typename T>
class Rational
{
  	public:
    	friend const Rational operator*(const Rational & lhs, const Rational & rhs)
        {
            return Rational(lhs.numberator() * rhs.numberator(),
                            lhs.denominator() * rhs.denominator())
            //或者是
            //在外部顶一个一个辅助函数，在这个里面直接调用辅助函数。
        }
};
```



==当我们编写一个类模板，而它所提供之与此模板相关的函数支持所有参数之隐式转换类型时，请将那些函数定义为类模板内部的友元函数。==



## 请使用traits classes表现各类信息

STL主要由用以表现容器、迭代器和算法的模板构成，但也覆盖若干工具性模板。

迭代器分类：

- `input`迭代器只能向前移动，一次一步，客户只可读取它们所指的东西，而且只能读一次。
- `output`迭代器只向前移动，一次一步，客户只能写它们所指的东西，而且只能写一次。
- `forward`迭代器是`input`和`ouput`的结合体，并且能够读写多次。
- `Bidirectional`迭代器，不只能够向前还能够向后。
- `random access`迭代器，可以在常量时间内向前或者向后移动任意距离。

上述的继承关系如下：

```c++
struct input_iterator_tag 				{};
struct output_iterator_tag				{};
struct forward_iterator_tag : public input_iterator_tag {};
struct bidirectional_iterator_tag : public forward_iterator_tag {};
struct random_access_iterator_tag : public bidirectional_iterator_tag {};
```



`iterator_traits`其实是一个用来判断类型的工具，

```c++
template<typename T>
struct iterator_traits;
```



我们看一个例子：

```c++
//实现移动迭代器的函数
template<typename IterT, typename DistT>
void advance(IterT& iter, DistT d)
{
    if(iter is a random access iterator) ?如何实现这里？;
    {
        iter += d;
	}
    else 
    {
        if(d >= 0) {while(d--) iter++;}
        else {while(d++) --iter;}
    }
}
```



`iterator_traits`的运作方式是，针对每一个类型`IterT`，在`struct `内一定声明某个`typdef`名为`iterator_category`。用来确认`IterT`的分类。所以我们可以写出：

```c++
template<typename IterT, typename DistT>
void advance(IterT& iter, DistT d)
{
    if(typeid(typename std::iterator_traits<IterT>::iterator_category) == typeid(std::random_access_iterator_tag))//这里是运行编译。
    {
        iter += d;
	}
    else 
    {
        if(d >= 0) {while(d--) iter++;}
        else {while(d++) --iter;}
    }
}
```



这会导致问题，因为`IterT`在编译阶段就会知道是什么类型，但是`if`却需要在运行时才会核定，不仅浪费时间，也会造成可执行文件膨胀。

那么我们可以通过重载函数的方式实现

```c++
template<typename IterT,  typename DistT>
void doAdvance(IterT & iter, DistT d, std::random_access_iterator_tag)
{
    cout << "randomaccess" << endl;
    //实现random_access迭代器
    iter += d;
    cout << *iter << endl;

}

template<typename IterT,  typename DistT>
void doAdvance(IterT & iter, DistT d, std::bidirectional_iterator_tag)
{
    cout << "Bidirectional" << endl;
    //实现bidirectional迭代器
    if(d >= 0) {while(d--) ++iter;}
    else {while(d++) --iter;}
    cout << *iter << endl;
}

template<typename IterT,  typename DistT>
void doAdvance(IterT & iter, DistT d, std::input_iterator_tag)
{
    cout << "Input" << endl;
    //实现Input迭代器
    if(d < 0)
    {
        throw std::out_of_range("Negative distance");
	}
    else
    {
        while(d--) ++iter;
    }
    cout << *iter << endl;
}

//这里就可以使用iterator_traits，从某种程度上在编译期进行了ifelse测试
template<typename IterT,  typename DistT>
void Advance(IterT& iter, DistT d)
{
    doAdvance(iter,d,typename std::iterator_traits<IterT>::iterator_category());
    //自动判断类型，调用不同的重载函数
}

//测试代码
vector<int> t({1,2,3,5});
auto it = t.begin();
Advance(it,2);

//输出
randomaccess
3
```



总结：

- 建立一组重载函数或者是函数模板，彼此间的差异只在于各自的`traits`参数。令每个函数实现码与其接受之`traits`信息相应和。
- 建立一个控制函数或函数模板，调用上述重载函数，传递`traits`提供的信息。



这是一种判断类型的好方法



==traits 类使得类型相关信息在编译期可用。==

==整合重载技术后，traits可能在编译器执行if...else测试==



## 认识模板元编程

template metaprogramming（模板元编程）是编写基于模板C++程序并执行于编译期的过程。是以C++写成、执行与C++编译器内的程序。一旦TMP程序结束执行，其输出，也就是从模板具现出来的若干C++源码，便会一如既往的编译。



上一部分使用`iterator_traits`就是TMP。



==模板元编程可将工作由运行期移往编译期，因而得以实现早期错误侦测和更高的效率==

==TMP可以被用来生成基于政策选择组合的客户定制代码，也可用来避免生成对某些特殊类型并不适合的代码==



