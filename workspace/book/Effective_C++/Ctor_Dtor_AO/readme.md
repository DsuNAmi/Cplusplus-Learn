# 了解C++默默编写并调用了哪些函数

对于一个空类，C++处理完之后。编译器会自动为它声明一个copy构造函数、一个copy assignment操作符和一个析构函数。

例如：

```c++
//如果写下一个空类
class Empty {};


//那么实际上等同于
class Empty{
    public:
		Empty() {}
    	Empty(const Empty & rhs) {}
    	~Empty(){} //non-virtual dtor
    	Empty & operator=(const Empty & rhs)
};
//只有这些函数被调用的时候，才会创建出来
```



如果一个类其中声明了一个构造函数，编译器于是不再为它创建默认构造函数。

```c++
template <typename T>
class NamedObject
{
    public:
        NamedObject(const char * name, const T & value);
        NamedObject(const std::string & name, const T & value);

    private:
        std::string nameValue;
        T objectValue;
};


NamedObject<int> no1("Smallest Prime Number",2);
NamedObject<int> no2(no1); //调用copy构造函数
```



其中`NamedObject`既没有声明copy构造函数，也没有copy assignment操作符。所以编译器会为它自动创造那些函数（==如果它们被调用的话==）。



编译器生成的copy构造函数必须以`no1.nameValue, no1.objectValue`为初值设定`no2.nameValue, no2.objectValue`。但是`nameValue`的类型是`string`，其本身有一个copy构造函数。所以`no2.nameValue`的初始化方式是调用`string`的copy构造函数并以`no1.nameValue`为实参。另一个是`int`，是内置类型。所以`no2.objectValue`会以**拷贝`no1.objectValue`内的每一个`bits`**来完成初始化。



编译器所生的copy assignment操作符，其行为和copy构造函数如出一辙，但一般而言只有当生出的代码合法且有适当机会证明它有意义。万一有一个不符合，编译器会拒绝生成`operator=`

举个例子：

```c++
template <typename T>
class NamedObject
{
    public:
        //以下构造函数不再接受一个const名称，因为nameValue
        //如今是一个reference-to-const string.
        NamedObject(std::string & name, const T & value);

        //假如未声明operator=

    private:
        std::string& nameValue;
        const T objectValue;
};


std::string newDog("persephone");
std::string oldDog("Satch");
NamedObject<int> p(newDog,2);
NamedObject<int> s(oldDog,36);

p =  s; //这段代码能否通过编译器检测？答案是否定的
```



这里涉及到一个问题，因为C++不允许引用指向的对象改变，也就是说，假如编译器能够通过这段代码，`p.nameValue`也不会指向`s.nameValue`。而是将`p.nameValue`所指对象的值改变了。



所以面对这个难题，C++的响应是拒绝编译那一行赋值动作。如果打算在一个含有引用成员函数的类支持assignment操作，你必须自己定义操作符。面对含有`const`成员的类，也是一样的道理。



最后还有一种情况：如果某个基类将copy assignment操作符声明为`private`，编译器将拒绝为其派生类生成一个copy assignment操作符。毕竟编译器为派生类所生的copy assignment操作符想象处理基类成分，但是又无法调用派生类无权调用的函数。



==编译器可以暗自创建默认构造函数、copy构造函数、copy assignment操作符，以及析构函数==



# 若不想使用编译器自动生成的函数，就该明确拒绝

因为编译器会自动生成一些成员函数，那么如果我现在要阻止这些函数，应该怎么做呢？

比如，我现在编写的类，不允许它进行拷贝构造和赋值构造。

其中一种方法是将上述的两类函数编写成`private`，但是这个做法并不是绝对安全，因为成员函数和友元函数还是能够调用这些私有的函数。



所以一个方法是**将成员函数声明为`private`但是故意不去实现它们**。



```c++
class HomeForSale
{
    public:
    	...;
    private:
    	...;
    	HomeForSale(const HomeForSale); //可以不写参数名字
    	HomeForSale & operator=(const HomeForSale);
};
```



同样，我们可以定义一个基类来专门做这件事情：

```c++
class Uncopyable
{
    protected:
        Uncopyable() {}                 //允许ctor dtor
        ~Uncopyable() {}
    private:
        Uncopyable(const Uncopyable &);  //阻止copy 
        Uncopyable & operator=(const Uncopyable &);
};

class HomeForSale : private Uncopyable
{
    
};
```



该类的涉及非常巧妙，包括不一定得以`public`继承它，以及`uncopyable`得析构函数不一定得是`virtual`。但是因为是基类，所以还是会造成多重继承的问题。



==为驳回编译器自动提供的机能，可将相应的成员函数声明为`private`并且不予实现。或者是使用像`Uncopyable`这样的基类也是一种办法。==



# 为多态基类声明virtual析构函数

有多种做法可以记录时间，设计一个类和一些派生类作为不同的计时方法，合情合理

```c++
class TimeKeeper
{
    public:
    	TimeKeeper();
    	~TimeKeeper();
    	...;
};

class AtomicClock : public TimeKeeper {}; 		//原子钟
class WaterClock  : public TimeKeeper {};		//水钟
class WristClock  : public TimeKeeper {};		//腕表
```



我们可以设计一个`Factory`来返回一个基类指针来指向派生类对象。

```c++
TimeKeeper * getTimeKeeper();

TimeKeeper ptk = getTimeKeeper();
delete ptk;
```



但是目前来说，基类没有一个`virtual dtor`。

C++指出，当派生类对象经由一个基类被删除，而基类带着一个`non-virtual`析构函数，其结果未有定义——实际执行时通常发生的事对象的派生成分没有被销毁。



解决该问题的方法就是在基类使用虚析构函数，这样就能消除包括派生类的部分。

```c++
virtual ~TimeKeeper();
```



但是如果反过来，如果一个类不准备当基类，而想让他的析构函数为虚的，往往不太好。来看一个例子：

```c++
class Point
{
    public:
    	Point(int xCoord, int yCoord);
    	~Point();
    private:
    	int x, y;
};
```



假设`int`占用`32bits`，那么这个类可以当作是一个`64bits`量。



而出现虚函数，则对象会携带某些信息，主要在运行期间决定哪一个`virtual`函数被调用。这份信息通常是一个所谓的虚指针指出。虚指针指向一个由函数指针构成的数组，成为虚表。没有个带有虚函数的类都有一个相应的虚表。当调用虚函数时，调用的时虚指针在虚表中指向的那个函数。

但是在64位的电脑中，虚指针占8个字节，所以本来的`64bits`量变成了`128bits`量。这样就不能放在一个`64bits`的缓存中，也不和其他语言的结构相同，也失去了移植性。



所以许多人的心得是：**只有当类内至少含有一个虚函数，才会声明虚析构函数**



但是即使是类完全不带虚函数，还是有肯能被非虚析构函数问题。

```c++
class SpecialString : public std::string
{
  	//看似无害，实则问题很大  
};

SpecialString * pss = new SpecialString("Impending Doom");
std::string * ps;
ps = pss;


delete ps;		//SpecialString的资源会泄露，因为其析构函数没有被调用
```



对于抽象类来说，如果想拥有一个没有纯虚函数的抽象类，那么应该如何声明？

```c++
class AWOV
{
  	public:
    	virtual ~AWOV() = 0;			//声明纯虚函数
};

//必须为这个析构函数提供定义
AWOV::AWOV() {}
```



析构函数的运行方式是，最深层的那个派生类的析构函数先调用，然后是每一个基类的析构函数调用。





==带多态性质的基类应该声明一个虚析构函数。如果类带有任何虚函数，那么就应该声明一个虚析构函数==

==类的设计目的如果不是为了作为基类使用，或者不是为了多态使用，那么就不应该声明虚析构函数==



# 别让异常逃离析函数

C++并不禁止析构函数吐出异常，但它不鼓励你这么做。

```c++
class Widget
{
    public:
        ~Widget() {} //假设这可能会吐出一个异常

};

void DoSomething()
{
    std::vector<Widget> v;
}//v在这里被自动销毁
```



简单来说，如果`v`有10个对象，那么在销毁第一个对象的时候抛出异常，很有可能导致程序结束执行或者是不明确的行为，这样剩下的9个资源就都泄露了。

但是如果现在有一个要求必须要在析构时抛出异常，该怎么办？

```c++
//假设要连接数据库
class DBConnection{
  	public:
    	...;
    	static DBConnection create();
    
    	void close(); //关闭连接，失败则抛出异常
};
```



为了确保客户不忘记在`DBConnection`上调用`close()`。一个合理的想法是在析构函数中调用该函数。

```c++
class DBConn{
    public:
    	~DBConn()
        {
            db.close();
        }
    private:
    	DBConnection db;
};

{
    DBConn dbc(DBConnection::create()); //如果close异常？
}
```



如果抛出异常，那么就会很麻烦；如果没有抛出异常，那其实还好。有两个方法解决必须抛出异常的问题：

- 如果`close`抛出异常就结束程序。通过`abort()`函数完成。

```c++
DBConn::~DBConn()
{
    try{db.close();}
    catch(...)
    {
        //制作转运记录，记下close的调用失败。
        std::abort();
    }
}
```

- 吞下因调用`close`发生的异常

```c++
DBConn::~DBConn()
{
    try {db.close();}
    catch(...)
    {
        //制作运转记录，记下close的调用失败。
    }
}
```



但是上面两种方法都不能对**导致close抛出异常**的情况做出反应。



我们来看另一种比较好的解决策略。可以把处理权转交给客户。

```c++
class DBConnection
{
    public:
        static DBConnection create();

        void close();
};

class DBConn
{
    public:
        void close()
        {
            db.close();
            closed = true;
        }

        ~DBConn()
        {
            if(!closed)
            {
                try
                {
                    db.close();
                }
                catch(exception)
                {
                    //记录错误
                    //终止程序或者是吞下异常
                }
            }
        }

    private:
        DBConnection db;
        bool closed;

};
```



本例要说的是，由客户自己调用`close`并不会对他们带来负担，而是给他们一个处理错误的机会，否则它们没机会响应。如果它们不认为这个机会有用，可以忽略它，依赖析构函数去调用。如果真有错误发生，客户没有立场抱怨，毕竟它们有机会处理该问题，而它们选择了放弃。



==析构函数绝对不要吐出异常。如果一个析构函数调用的函数可能抛出异常，析构函数应该捕捉任何异常，然后吞下它们或者是结束程序。==

==如果客户需要对某个操作函数运行期间的异常做出反应，那么类应该提供一个普通函数（而非在析构函数中）执行操作。==





# 绝不在构造和析构过程中调用virtual函数

假设有一个类继承体系，用来塑膜股市交易。如买进、卖出的订单等等。

对于不同的交易，我们需要在创建时记录不同的内容。

```c++
class Transaction{
    public:
        Transaction();
        virtual void logTransaction() const = 0;            //做出一份因类型不同而不同的日志记录
};

Transaction::Transaction()
{
    //一系列操作
    logTransaction();//最后是记录这笔交易
}


class BuyTransaction : public Transaction
{
    public:
        virtual void logTransaction() const;
};

class SellTransaction : public Transaction
{
    public:
        virtual void logTransaction() const;
};

//现在进行对象创建
BuyTransaction b;//会发生什么？
```



当我们试图创建一个派生类对象时，会先调用基类的构造函数。而此时调用的`logTransaction()`函数也是基类中的`logTransaction()`。

基类构造期间虚函数绝不会下降到派生类阶层。取而代之的，对象就像是隶属基类一样。换句话说，基类构造期间，虚函数不是虚函数。在派生类对象的基类构造期间，对象的类型是基类而不是派生类。

相同道理也同样适用于析构函数。一旦派生类析构函数开始执行，对象类的派生类成员变量便呈现未定义值，所以C++视它们不再存在。

上述的例子C++程序可以侦测出来，但是有些情况并不是那么容易检测出来。

```c++
class Transaction{
    public:
        Transaction(){init();}
        virtual void logTransaction() const = 0;            //做出一份因类型不同而不同的日志记录
    private:
        void init()
        {
            logTransaction();
        }
};
```



那么我们如何解决该节开头所提到的问题呢？

一种做法是在`Transaction`内将`logTransaction`改为`non-virtual`函数，然后要求派生类构造函数传递必要信息给`Transaction`函数，然后那个构造函数就可以调用`non-virtual`函数。

```c++
class Transaction{
    public:
        explicit Transaction(const std::string & logInfo);
        void logTransaction(const std::string & logInfo) const;            //做出一份因类型不同而不同的日志记录    
};

Transaction::Transaction(const std::string & logInfo)
{
    //一系列操作
    // logTransaction();//最后是记录这笔交易
    logTransaction(logInfo);
}


class BuyTransaction : public Transaction
{
    public:
        BuyTransaction() : Transaction(createLogString()) {}
    private:
        static std::string createLogString();//这里
    //令此函数为static，也就不可能意外指向“初期未成熟之BuyTansaction”对象内尚未初始化
    //的成员变量。正是因为那些成员变量处于未定义状态，所以在基类构造和析构期间调用的虚函数不可下降
    //到派生类。
};
```



换句话说，你无法使用虚函数从基类向下调用，在构造期间，可以由**令派生类将必要的信息向上传递至基类构造函数**。



==在构造和析构期间不要调用虚函数，因为这类调用从不下降至派生类（比起当前执行构造函数和析构函数的那层）==



# 令operator=返回一个reference to * this

为了实现**连锁赋值**，赋值操作符必须返回一个reference指向操作符的左侧实参。

```c++
class Widget
{
    public:
        Widget & operator=(const Widget & rhs)
        {
            return *this;
        }
};

//该协议不仅适用于以上的标准形式，也适用于所有的运算
class Widget
{
    Widget & operator+=(const Widget & rhs) //包括+= -= *=
    {
        return *this;
    }

    Widget & operator=(int rhs)
    {
        return *this;
    }
};
```



==令赋值操作符返回一个reference to *this==



# 在operator=中实现自我赋值

自我赋值发生在对象被赋值给自己时，有明显的自我赋值和潜在的自我赋值。

```c++
w = w;
a[i] = a[j]; //潜在的自我赋值
*px = *py;

class Base{};
class Derived : public Base{};
void doSomething(const Base & pb, const Derived & pd);//pb和pd可能是同一对象
```



假如现在建立一个类用来保存一个指针指向一块动态分配的位图(bitmap)

```c++
class Bitmap {...;};
class Widget
{
    ...;
    private:
    	Bitmap * pb; //指针，指向一个从heap分配而得的对象
};
```

我们先来看一段自我赋值的代码

```c++
Widget & Widget::operator=(const Widget & rhs)
{
    delete pb;
    pb = new Bitmap(*rhs.pb);
    return *this;
}
```



如果`rhs.pb`和`this->pb`指向同一个对象，那么指针就会被指向一个已经销毁的对象。

我们需要加入一个**证同测试**：

```c++
Widget & Widget::operator=(const Widget & rhs)
{
    if(this == &rhs) return *this;
    delete pb;
    pb = new Bitmap(*rhs.pb);
    return *this;
}
```



令人高兴的是，让operator=具备**异常安全性**往往自动获得**自我赋值安全**的回报。因此越来越多的人对自我复制的态度是不去管他，把焦点放在实现异常安全性上。

如以下代码：

```c++
Widget & Widget::operator=(const Widget & rhs)
{
    Bitmap * pOrig = pb;
    pb = new Bitmap(*rhs.pb);
    delete pOrig;  //删除原先的pb
    return *this;
}
```



现在，如果`new Bitmap`抛出异常，但是原先的pb还是保持原状。但是这段赋值还是能够处理自我复制，只是效率没有那么高。



为了提升效率，可以先估计证同测试的频率有多高？因为测试需要成本，会使代码变大，并导入一个新的控制流分支，降低执行速度。



或者我们使用`copy and swap`方法。

```c++
class Widget
{
    void swap(Widget & rhs); //交换*this 和 rhs的数据
};


Widget & Widget::operator=(const Widget & rhs)
{
    Widget temp(rhs);
    swap(temp);
    return *this;
}
```



==确保对象自我赋值时operator=有良好行为。其中技术包括比较 来源对象 和 目标对象 的地址、精心周到的语句顺序以及copy-and-swap==

==确定任何函数如果操作一个以上的对象，而其中多个对象是同一个对象时，其行为仍然正确==



# 复制对象时勿忘其每一个成分

现在我们考虑一个类用来表现顾客，其中手工写出copying函数，使得外界对它们的调用会被logged下来：

```c++
void logCall(const std::string & funcName); //制造一个LOG ENTRY

class Customer
{
    public:
        Customer(const Customer & rhs);
        Customer & operator=(const Customer & rhs);
    private:
        std::string name;
};


Customer::Customer(const Customer & rhs)
:name(rhs.name)
{
    logCall("Customer copy constructor");
}

Customer & Customer::operator=(const Customer & rhs)
{
    logCall("Customer copy assignment operator");
    name = rhs.name;
    return * this;
}
```



目前来说，这里的一切都很好，直到另一个对象加入进来。

```c++
class Date {...;};
class Customer{
    public:
    //同前：
    private:
    	std::stirng name;
    	Date LastTransaction;
}
```



此时如果不修改copy函数和copy assignment函数，那么就不会复制Date对象，因为并没有写（覆盖了默认的复制构造函数）。

如果一旦发生继承，那么可能会造成一个潜藏的危机。

```c++
class PriortyCustomer : public Customer
{
    public:
        PriortyCustomer(const PriortyCustomer & rhs);
        PriortyCustomer& operator=(const PriortyCustomer & rhs);
    private:
        int priority;
};

PriortyCustomer::PriortyCustomer(const PriortyCustomer & rhs)
:priority(rhs.priority)//这里没有传递参数给基类。
{
    logCall("PriorityCustomer copy constructor");
}

PriortyCustomer & PriortyCustomer::operator=(const PriortyCustomer & rhs)
{
    logCall("PriorityCustomer copy assignment operator");
    priority = rhs.priority;
    return *this;
}
```



这段代码，派生类只复制了派生类里面的东西，而没有复制基类的东西。

修改：

```c++
class PriortyCustomer : public Customer
{
    public:
        PriortyCustomer(const PriortyCustomer & rhs);
        PriortyCustomer& operator=(const PriortyCustomer & rhs);
    private:
        int priority;
};

PriortyCustomer::PriortyCustomer(const PriortyCustomer & rhs)
:Customer(rhs),priority(rhs.priority)
{
    logCall("PriorityCustomer copy constructor");
}

PriortyCustomer & PriortyCustomer::operator=(const PriortyCustomer & rhs)
{
    logCall("PriorityCustomer copy assignment operator");
    Customer::operator=(rhs);
    priority = rhs.priority;
    return *this;
}
```



当你编写一个copying函数：

- 复制所有local成员变量
- 调用所有基类适当的copying函数。

==copying函数应确保复制 对象内的所有成员变量函数 及 所有基类成分==

==不要尝试以某个copying函数去实现另一个copying函数。应该将共同机能放进第三个函数中，并由两个copying函数共同调用==

