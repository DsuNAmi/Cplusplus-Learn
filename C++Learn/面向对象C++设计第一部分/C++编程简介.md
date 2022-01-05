# C++编程简介

>面向对象是一种观念

C++中的每一个单元都是一个Class

==new C -> C with **Class** -> C++==

> 程序就是处理数据的方法

类有两种类型：

1. 带指针的：complex
2. 不带指针的 string

#### C++代码的基本形式

头文件   `.h header` -> 文件  `.cpp`  <- 标准库  `.h STL` 

* 标准库 `include <name.h>`
* 头文件 `include "name.h"`

==也可能没有延长名或者是.hpp==



> 头文件的防御式声名
>
> ```c++
> #name.h
> //如果没有定义过`__NAME__`,那么就定义`__NAME__`
> #ifndef __NAME__ #自己定义的名字
> #define __NAME__
> ...
> ...
> #endif
> ```



> 头文件的布局
>
> ```c++
> #头文件的布局
> #complex.h
> #ifndef __COMPLEX__
> #define __COMPLEX__
> 
> #include <math.h>
> //前置声名
> class ostream;
> class complex;
> //前置声名
> 
> complex &
>     __doapl (complex* ths, const complex& r);
> 
> //类声明
> class complex
> {
>   ...  
> };
> //类声明
> 
> //类定义
> complex::function...
> //类定义
> 
> #endif
> ```



一个**class** 由两个部分组成：

* class head
* class body



类设计的一些规范：

- [ ] 数据一定放在`private`
- [ ] 参数使用`reference`传递
- [ ] 根据情况给函数加上`const`，应该加就必须加
- [ ] 返回值尽量使用`reference`传递
- [ ] 构造函数使用初始化列表进行赋初值



> 模板（template）`template<typename T>`
>
> 模板会带来代码的膨胀，但是代码的膨胀是必要的
>
> ```c++
> template<typename T>
> class name
> {
> T var;  //目前还不知道T是什么
> };
> 
> //会产生两份代码
> name<double> n1;
> name<int> n2;
> ```
>
> 函数模板  `function template`
>
> ```c++
> stone r1(2,3), r2(3,3), r3;
> r3 = min(r1,r2);//不必指出是什么类型
> 
> template <class T>//class 和 typename 在这里一样
> inline const T& min(const T& a, const T& b)
> {
>     return b < a ? b : a;//调用stone的重载
> }
> 
> //编译器会对函数模板进行参数推导
> ```
>
> 



> 内联函数 `inline`
>
> 在函数内部定义，`inline`函数比较快
>
> ==函数复杂就不能是`inline`==
>
> 即使定义了`inline`也不一定会变成`inline`
>
> ```c++
> class name
> {
>   double real() {} //建议编译器为inline 
> };
> 
> //如果在函数外面，需要加-inline-关键字
> inline double real() {}
> 
> ```



> 访问级别
>
> ```c++
> class name
> {
>     public: //可以在任意位置定义
>     	//能够被外部访问
>        //一般存放函数
>     private:
>     	//不能被外部范围
>     	//一般存放数据
>     protected:
>     	//与继承有关
>     
>     public://不一定分要在一个位置声名-public-
> };
> ```



> 构造函数`constructor`
>
> 1. 函数名称和类名必须相同
> 2. 参数可以有默认值，参数可选
> 3. 没有返回值类型
> 4. 尽量使用初始化列表赋==初值-====初始化==
> 5. 不能直接调用构造函数，构造函数在创建对象时被调用
> 6. 一个类可以有多个构造函数，`overloading`
>
> 数值的初始化包含了初始化和赋值，使用初始化列表能够提高程序编译的效率。
>
> ```c++
> //为什么下面两个函数不能重载
> class complex
> {
>   public:
>     complex(double r = 0, double i = 0)
>         : re(r), im(i) {}
>     complex() : re(0), im(0) {}
>   private:
>     double re, im;
> };
> 
> complex c1;
> complex c2();
> 
> //因为其中一个函数已经有了默认值，这样重载会让编译器无法确定调用哪一个构造函数
> ```
>
> ==多个构造函数的重载为什么存在？==
>
> 编译器会编译函数名相同的函数为不同的内容，在编译器的眼里是不一样的。
>
> ==构造函数放入`private`== -> 正确语法
>
> 1. 不能在外部创建对象
>
> 经典设计模式：构造函数放入`private`
>
> `Singleton` ==单例==
>
> ```c++
> 
> class A{
>     public:
>     	static A& getInstance();
>     	setup() {...}
>     private:
>     	A();
>     	A(const A& rhs);
>     	...
> };
> 
> A& A::getInstance()
> {
>     static A a;
>     return a;
> }
> 
> #调用方法
> A::getInstance();
> A::getInstance().setup();
> ```
>
> 



> 常量成员函数`const member function`
>
> 在`)` 后，`{`前加`const`
>
> 函数分为两种：
>
> 1. 改变数据内容的
> 2. 不改变数据内容的（不改变数据的->  `+const`）
>
> 在创建对象的时候加`const`表示对象是不能改变的。
>
> **如果没有在不改变的函数加`const`，在创建常量对象的时候会产生矛盾**，所以一定要在不改变数据内容的函数后面加上`const`。这样就能创建常量对象
>
> ```c++
> class complex{
>   ...
>   double real() const {return re;}
>   ...
> };
> 
> const complex c1(2,1);
> 
> ```



> 参数传递   ==尽量不用值传递==
>
> * `pass by value`，传递整个值，整包传过去，4Bytes就传4Bytes。
> * `pass by reference`，引用在底部就是指针，相当于传指针的速度。==尽量所有的参数都传引用== 。传指针会改变原始的值，但是修改值的后果不确定。但是如果传参数是为了提高程序效率，使用`const complex&`保证传递的参数不会被修改。如果函数尝试修改，那么编译就会出错。
> * `pass by point`，C的特性
>
> 一个指针==4Bytes==
>
> ```c++
> complex& operator += (const complex&); //reference
> ```
>
> 返回值的传递也==尽量==传递引用，有的情况不能传递引用
>
> ```C++
> inline complex& //使用引用
> __doapl(complex* ths, const complex& c)
> {
>     //第一参数将被改变
>     //第二参数不会被改变
>     ths->re += r.re;
>     ths->im += r.im;
>     return *ths;
>     //在函数里面创建的变量，对象，如果要返回这个变量或者对象，这个时候就不能使用引用的传递方式，因为函数结束的时候，会自动销毁这个变量或者对象，这个时候就不能传引用。
> }
> 
> inline complex&
> complex::operator += (const complex& r)
> {
>     return __doapl (this,r);
> }
> ```
>
> 



> 友元 `friend`
>
> 友元函数可以取得类的私有变量，友元可以解==封装==
>
> ```c++
> class complex
> {
>   private:
>     double re, im;
>   friend complex& __doapl(complex*, const complex&);
> };
> 
> inline complex& __doapl(complex* ths, const complex& r)
> {
>     ths->re += r.re;
>     ths->im += r.im;
>     return *ths;
> }
> 
> 
> ```
>
> 相同`class`的各个`objects`互为`friends`
>
> ```c++
> class complex
> {
>   public:
>     complex (double r = 0, double i = 0)
>         : re(r), im(i)  {}
>     int func(const complex& param)
>     { return param.re + param.im;} //这里直接调用了另一个类的私有变量。因为默认是友元了。
>   private:
>     double re, im;
> };
> ```



>操作符重载
>
>==所有的成员函数都有一个隐藏的参数`this`==
>
>==为什么返回是`return by reference`?==
>
>因为传递者无需知道接收者是以`reference`形式接收，所以调用函数的人就不需要关心传递什么样的值。
>
>因为返回的`this`的值，
>
>`c3 += c2 += c1` 使用引用可以处理连串赋值的情况。
>
>```C++
>//成员函数的方式
>//编译器看待操作符的方式
>c2 += c1;
>=>;//二元操作符
>inline complex& complex::operator +=(this,const complex& r)
>{
>    //this 是不能写的，但是编译器会存在一个this
>    return __doapl(this,r);
>}
>
>//标准库中的命名
>//__doapl
>inline complex& __doapl(complex* ths, const complex& r)
>{
>    ths->re += r.re;
>    ths->im += r.im;
>    return *ths;
>}
>
>//
>c2 += c1;
>c3 += c2 += c1;//?
>
>
>//非成员函数
>c2 = c1 + c2;//这种情况的写法
>inline complex operator+(const complex& x, const complex& y)
>{
>    //如果只有一个参数就是正号，两个就是加法
>	return complex(real(x) + real(y),
>                   imag(x) + imag(y));//real(), 和 imag()是定义的函数
>}
>
>c2 = c1 + 5;
>inline complex operator+(const complex& x, double y)
>{
>    return complex(real(x) + y, imag(x));
>}
>
>//为什么不传引用，因为对象是在函数内部定义的，函数结束这个变量就会消失，是一定不能传reference的
>
>#include<iostream>
>//同样是为了连写
>ostream& operator<< (ostream& os, const complex& x)
>{
>    //这个os对象是不能加const的，因为每一个输出都在改变cout的状态。
>	return os << '(' << real(x) << ','
>              << imag(x) << ')';
>}
>
>//这样写不能连写
>void operator<<(ostream& os, const complex& x)
>{
>    ...;
>}
>
>```



>`typename();` 创建一个临时对象，没有名称。
>
>临时变量的生命周期只有一行
>
>```c++
>int(7);
>int name = 7;
>```



> 拷贝构造函数和赋值构造函数本来就存在
>
> 只有在默认情况下不能满足需求才自己写这两个函数
>
> ==比如类中有指针的情况下==
>
> ```C++
> class String
> {
>     public:
>     	String(const char* cstr = 0);
>     	String(const String& str);//拷贝构造函数
>     	String& operator=(const String& str);//赋值构造函数
>     	~String();
>     	char* get_c_str() const {return m_data;}
>     private:
>     	char* m_data;
> };
> ```
>
> Big Three: 三个特殊函数
>
> * 拷贝构造函数
> * 赋值构造函数
> * 析构函数
>
> ```c++
> inline String::String(const char* cstr = 0)
> {
>     if(cstr) 
>     {
>         m_data = new char[strlen(cstr) + 1];
>         strcpy(m_data,cstr);
>     }
>     else
>     {
>         m_data = new char[1];
>         *m_data = '\0';//没有内容的字符串也要给一个'\0'
>     }
> }
> 
> inline String::~String()
> {
>     delete[] m_data;//不然会造成内存泄漏
> }
> ```
>
> 对于有指针的类，存在浅拷贝和深拷贝
>
> ```c++
> //拷贝构造函数
> inline String::String(const String& str)
> {
>     m_data = new char[stelen(str.m_data) + 1];
>     strcpy(m_data,str.m_data);
> }
> 
> //下面两个表达的意思完全相同
> String s2(s1);
> String s2 = s1;
> 
> //赋值拷贝函数
> inline String& String::operator=(const String& str)
> {
>     //检测自我赋值
>     if(this == &str) return *this;
>     //如果不进行自我检测，那么在delete的时候就会把同一个指针指向的空间给释放了，这样就没有这个内容了，后面的语句也无法执行了
>     delete[] m_data; //要把自己的空间给腾出来
>     m_data = new char[strlen(str.m_data) + 1];
>     strcpy(m_data,str.m_data);
>     return *this;
> }
> 
> s2 = s1;
> ```



>栈  `stack`
>
>> 是存在于某作用域的一块内存空间。例如当你调用函数，函数本身即会形成一个  `stack`   用来放置它所接收的参数，以及返回地址。
>>
>> 在函数本体内声名的任何变量，其所使用的内存块都取自上述  `stack` 
>
>堆  `heap`
>
>> 是指由操作系统提供的一块   `global`  内存空间，程序可动态分配从某种获得若干块
>
>```c++
>class Complex {};
>...
>{
>    Complex c1(1,2);// c1 所占的空间是 stack
>    Complex* p = new Complex(3); // Complex(3)是个临时对象。
>    
>    //离开这个作用域，c1的生命周期就结束了，但是要结束c2的生命还需要自己手动delete
>    
>    static Complex c2(1,2);
>    //如果加了static。生命在作用域结束之后仍然存在，直到整个程序结束。
>    
>    //全局对象的生命周期也是整个程序，直到程序结束。
>}
>```





> `new `  先分配内存，再调用构造函数
>
> ```c++
> Complex * pc = new Complex(1,2);
> //编译器转化
> /*  */
> Complex *pc;
> void* mem = operator new(sizeof(Complex));//调用malloc(n),两个double
> pc = static_cast<Complex*>(mem);//转换数据类型
> pc->Complex::Complex(1,2);//构造函数
> //全名：Complex::Complex(pc,1,2);
> /*  */
> 
> ```
>
> `delete`  先调用析构函数，再释放内存
>
> ```c++
> delete ps;
> //编译器转换
> String::~String(ps);
> operator delete(ps); // 其内部调用free(ps)
> ```
>
> `array new` 一定要对应 `array delete`



> `static`  
>
> 在数据或者函数前面加上 `static` 关键字表示静态
>
> 非静态数据每创建一个对象都会分配一个内存。也就是有多份
>
> 非静态函数调用的时候会传入调用对象的地址，但是函数还是只有一份。
>
>
> 静态数据与对象脱离，不属于某一个对象，该数据只有一份。
>
> 静态函数也是只有一份。静态函数没有 `thisPoint` ，主要去处理静态的数据。
>
> ```c++
> 
> //非静态情况
> complex c1,c2,c3;
> c1.real() --> complex::real(&c1);
> c2.real() --> complex::real(&c2);
> 
> 
> //静态情况
> class Account
> {
>   public:
>     static double m_rate;//声名
>     static void set_rate(const double& x) {m_rate = x;}
> };
> 
> double Account::m_rate = 8.0;//在类外定义静态成员变量,可以给初值也可以不给初值
> 
> int main()
> {
>     Account::set_rate(5.0);//调用静态函数的第一个方式，通过class name调用
>     Account a;
>     a.set_rate(7.0);//调用静态函数的第二个方式，通过对象调用
> }
> 
> //单例,把自己放在私有成员里
> class A
> {
>     public:
>     	static A& getInstance(return a;);
>     	setup() {;}
>    	private:
>     	A();
>     	A(const A& rhs);
>     	static A a;
> };
> 
> class A{
>     public:
>     	static A& getInstace();
>     	setup(){;}
>     private:
>     	A();
>     	A(const A& rhs);
> };
> 
> A& A::getInstance()
> {
>     static A a;//如果没有人调用单例，这个对象就不会出现，如果有人调用，那么就会创建，并且也只有一份
>     return a;
> }
> ```



> `cout `
>
> 因为cout 重载了很多方法， 所以才能针对不同的数据类型进行打印
>
> ```c++
> class _IO_ostream_withassign : public ostream
> {
>     ...;
> };
> 
> extern _IO_ostream_withassgin cout;
> 
> 
> ```



> `namespace`
>
> 所有的东西被包装在这个命令空间里面
>
> ```c++
> namespace std
> {
>     
> };
> 
> using namespace std;//相当于打开整个封锁
> using std::cout;
> ```



> `other` 
>
> `operator typename() const` 自定义类型转换函数
>
> `pointer-like` `object`
>
> `function-like` `object`
>
> `Standard Library` 

#### 面向对象设计

>`Composition` 复合 `has - a`
>
>类里面有另一种类（我有一个类别）
>
>两个类的生命周期是同步的
>
>```c++
>//Adapter的 设计模式
>//那么queue就是adapter，改一个已经实现的很好的类
>template <class T>
>class queue
>{
>  protected:
>    deque<T> c;//has - a
>  public:
>    //以下完全利用c的操作函数完成
>    bool empty() const {return c.empty();}
>    size_type size() const {return c.size();}
>    reference front() {return c.front();}
>    reference back() {return c.back();}
>    //
>    void push(const value_type& x) {c.push_back(x);}
>    void pop() {c.pop_front();}
>};
>//A拥有B并且所有的功能都让B来做
>
>```
>
>`queue`   ->   `deque`
>
>复合关系下构造函数和析构的关系
>
>==构造函数由内而外==
>
>`Container` 的构造函数先调用 `Component` 的==默认==构造函数再执行自己的构造函数（存在多个构造函数就只能调用默认构造函数，但是可以自己编写代码实现调用构造函数）
>
>```c++
>Container::Container(): Component() {}
>```
>
>类似打地基
>
>==析构由外而内==
>
>`Container` 的析构函数首先执行自己的，然后才调用 `Component` 的析构函数
>
>```C++
>Container::~Container() {...;~Component();}
>```
>
>有的类似剥皮的过程

>`Delegation` 委托
>
>一个类中包含另一个类的指针
>
>引用的复合  `Composition by reference`
>
>两个类的生命周期是不同步的
>
>编译防火墙
>
>==引用计数==
>
>多个对象共享一个字符串
>
>要改变内容的时候，`copy` 一份去改动。==copy on write==
>
>
>
>```c++
>class StringRep;
>class String{
>  	public:
>    	String();
>    	String(const char* s);
>    	String(const String& s);
>    	String &operator=(const String& s);
>    	~String();
>    private:
>    	StringRep* rep; //pimpl,有另一个类的指针。
>    //该类只实现对外的接口，所有的功能实现在StingRep中实现
>    //那么StringRep中怎么变动都不影响该类
>    //编译防火墙
>    
>};
>
>namespace{
>    class StringRep{
>        friend class String;
>        StringRep(const StringRep& );
>        ~StringRep(){};
>		int count;
>        char * rep;
>    }
>}
>```
>
>

>`Inheritance`  继承， `is-a`
>
>`public`   `private`   `protected` 三种继承
>
>`is-a` 就表示 ==是一种==的概念
>
>子类的对象有==父类的成分==
>
>
>
>```c++
>struct _List_node_base
>{
>  	_List_node_base* _M_next;
>    _List_node_base* _M_prev;
>};
>
>template<typename _Tp>
>struct _List_node : public _List_node_base
>{
>  	_Tp _M_data; //父类的数据完全继承过来
>};
>```
>
>构造函数与析构函数的顺序
>
>==构造函数由内而外==
>
>子类的构造函数先调用父类的==默认==构造函数
>
>```c++
>Derived::Derived(): Base(){}
>```
>
>
>
>==析构函数由外而内==
>
>子类先调用自己的析构函数，然后调用父类的析构函数。
>
>其中必须满足==父类的析构函数必须是虚函数，不然会出现未定义行为==
>
>```c++
>Derived::~Derived(){...;~Base();}
>```

>`virtual` 虚函数，继承搭配虚函数来使用
>
>语法：`virutal void draw() const = 0`
>
>在继承关系里面，数据（==内存继承==）可以被继承下来，函数的继承继承的是==调用权==
>
>根据虚函数分类，可以分成三类
>
>* `non-virtual` 你不希望派生类重新定义（override，复写）它 `int objectID() const;`
>* `virtual` 你希望派生类重新定义它，而且它已经有默认定义 `virtual void error(const std::string& msg);`
>* `pure vitrual` 你希望派生类一定要重新定义它，你对它没有默认定义`virtual void draw() const = 0;`
>
>子类调用父类的函数，this指针传入的是子类对象的指针
>
>==template Method== (23个设计模式之一，写应用程序框架)

>##### 委托+继承
>
>```c++
>class Subject{
>  	int m_value;
>    vector<Observer*> m_views;
>    public:
>    	void attach(Observer* obs)
>        {
>            m_views.push_back(obs);
>        }
>    	void set_val(int value)
>        {
>            m_value = value;
>            notify();
>        }
>    	void notify()
>        {
>            for(int i = 0; i < m_views.size();++i)
>                m_views[i]->update(this,m_value)
>        }
>};
>
>class Observer
>{
>    public:
>    	virtual void updata(Subjuect* sub, int value) = 0;
>}
>```
>
>



