# 资源管理

所谓资源就是，一旦用了它，就必须还给系统。

C++程序最常使用的资源就是动态分配内存，但内存只是你必须管理的众多资源之一。

## 以对象管理资源

假设我们使用一个用来塑膜投资行为的程序库。

```c++
class Investment {...;}; 			//"投资类型"继承体系中的root class

Investment * creatInvestment()；		//返回指针，指向Investment继承体系内的动态分配对象。调用者有责任删除它
  
void f()
{
    Investment * pInv = createInvestment();
    ...;
    delete pInv;	//不一定能够执行这段代码
}
```

为确保 `createInvsetment`返回的资源总是被释放，我们需要将资源放进对象内，当控制流离开 `f`，该对象的析构函数会自动释放那些资源。

许多资源被动态分配于 `heap`内而后被用于单一区块或函数内。它们应该在控制流离开那个区块或函数时被释放。

标准程序库提供了 `auto_ptr`正是针对这种情形设计的产品。`auto_ptr`是一个**类指针对象**，也就是所谓的智能指针，其析构函数能够对其所指对象调用 `delete`。

来看个例子，`auto_ptr`如何避免 `f`函数潜在的资源泄露的可能：

```c++
void f()
{
    std::auto_ptr<Investment> pInv(createInvestment());
    ...;
}
```

- **获得资源后理科放进管理对象内**。以上代码中 `createInvestment()`返回的资源被当作其管理者 `auto_ptr`的初值。实际上，**以对象管理资源**的观念常被称为**资源取得时便是初始化时机（Resource Acquisition Is Initialization; RAII）**。
- **管理对象运用析构函数确保资源被释放**。一旦对象被销毁就会自动调用析构函数。

`auto_ptr`会在被销毁时自动删除所指的对象，所以不要让多个 `auto_ptr`指向同一个对象，那么同一个对象会被删除多次。

为了解决上述问题，`auto_ptrs`有一个性质：**若通过copy构造函数或copy assignment操作符复制它们，它们会编程null，而复制所得的指针将取得资源的唯一拥有权！**

```c++
std::auto_ptr<Investment> pInv1(createInvestment());	//pInv1指向返回对象
std::auto_ptr<Investment> pInv2(pInv1);					//pInv2指向对象，pInv1为空
pInv1 = pInv2;											//pInv1指向对象，pInv2为空
```



虽然又保护机制，但是这样就不能复制指针。

`auto_ptr`的一个替代方案是**引用计数型智慧指针（Reference-counting smart pointer; RCSP）**

```c++
void f()
{
    ...;
    std::tr1::shared_ptr<Investment> pInv1(creteInvestment());	//pInv1指向返回的对象
    
    std::tr1::shared_ptr<Investment> pInv2(pInv1);				//pInv1和pInv2指向同一个对象
    
    ...;					//pInv1和pInv2被销毁，对象也同时被销毁。
}
```



`auto_ptr`和`tr1::shared_ptr`两者都是在其析构函数内做`delete`操作而不是`delete []`操作。但是分配数组还是能够通过编译。

```c++
std::auto_ptr<std::string> aps(new std::string[10]);
std::tr1::shared_ptr<int> spi(new int[1024]);
```



==为了防止资源泄露，请使用RAII对象，它们在构造函数中获得资源并在析构函数中释放资源==

==两个常被使用的RAII类分别是tr1::shared_ptr和auto_ptr。前者通常是最佳选择，因为其copy比较直观。若选择auto_ptr，复制动作会使他指向null==



## 在资源管理类中小心copying行为

假如我们使用C API函数处理类型为Mutex的互斥器对象，共有`lock,unlock`两个函数可以用

```c++
void lock(Mutex * pm);				//锁定pm所指的互斥器。
void unlock(Mutex * pm);			//将互斥器解除锁定
```



为了确保不会忘记将一个锁住的`Mutex`解锁，可能会希望建立一个来管理。这样的结构需要遵守RAII

```c++
class Lock
{
    public:
    	explicit Lock(Mutex * pm) : mutexPrt(pm) {lock(mutexPtr);}  //获得资源
    	~Lock() {unlock(mutexPtr);}									//释放资源
    private:
    	Mutex * mutexPtr;
}
```



客户队Lock的用法符合RAII

```c++
Mutex m;
...;
{
    Lock m1(&m);		//锁定互斥器
    ...;
}		//区块结束，调用析构函数，自动解除互斥器锁定

//如果发生复制，会发生什么事情？
Lock m11(&m);
Lock m12(&m);
```



当一个RAII被复制时，会发生什么事？

- **禁止复制**。许多时候允许RAII对象被复制并不合理。将`copying`操作声明为Private。就可以禁止复制。
- **对底层资源祭出引用计数法**。有时候我们希望保有资源，直到它的最后一个使用者被销毁。这种情况下复制RAII对象时，应该将资源的**被引用数**递增。`tr::shared_ptr`就是采用这种方式。
- **复制底部资源**。复制资源对象时，进行深度拷贝。
- **转移底部资源的拥有权**。类似`auto_ptr`的规则。



==复制RAII对象必须一并复制它所管理的资源，所以资源的copying行为决定RAII对象的copying行为==

==普遍而常见的RAII class copying行为是：抑制copying、施行引用计数法。不过其他行为也都可能被实现==



## 在资源管理类中提供对原始资源的访问

许多APIs直接指涉资源，除非永远不适用这样的APIs，否则就只得绕过资源管理对象直接访问原始资源。

来看一个例子：

```c++
std::tr1::shared_ptr<Investment> pInv(createInvestment());

//假设你希望以某个函数处理Investment对象，比如
int daysHeld(const Investment * pi);				//返回投资天数

//但是你这么调用它
int days = daysHeld(pInv);					//错误
//因为参数是Investment指针，而你传入的是tr1::shared_ptr<Investment> 对象。

//tr1::shared_ptr和auto_ptr都提供一个get()成员函数，用来执行显示转换，也就是它会返回智能指针内部的原始指针（的复件）
int days = daysHeld(pInv.get());			//可以

```



同样，其也重载了指针操作符，它们允许隐式转换至底部指针。

```c++
class Investment
{
    public:
    	bool isTaxFree() const;
    	...;
};

Investment * createInvestment();

std::tr1::shared_ptr<Investment> pi1(createInvestment());

bool taxble1 = !(pi1->isTaxFree());

std::auto_ptr<Investment()> pi2(createInvestment());

bool taxable2 = !((*pi2).isTaxFree()); 
```



有时候还是必须取得RAII对象内的原始资源，某些RAII类设计者是联想到**油脂涂在滑轨上**，做法是提供一个隐式转换函数。

```c++
FontHandle getFont();

void releaseFont(FontHandle fh);

class Font
{
    public:	
    	explicit Font(FontHandle fh) : f(fh) {}
    	~Font() {releaseFont(f);}
    private:
    	FontHandle f;
};
```

假设有大量的于字体相关的C API，它们处理的是`FontHandle`s，那么将`Font`转换成`FontHandle`会是一种很频繁的需求。那么就可以设计一个显示转换函数。

```c++
FontHandle get() const {return f;}	//显示成员函数
operator Fonthandle() const {return f;} //隐式成员函数
```



那么用户每次使用就必须调用`get()`。或者直接使用`Font f`。



==APIs往往要求访问原始资源，所以每一个RAII类应该提供一个取得其所管理资源的办法==

==对原始资源的访问可能经由显示转换或者是隐式转换。一般而言显示转换比较安全，隐式转换对客户比较方便。==



## 成对使用new和delete时要采取相同形式

来看一个错误例子：

```c++
std::string * stringArray = new std::string[100];
...;
delete stringArray;//100个中的99个对象没有删除，因为没有调用析构函数。
```



当你使用`new`，有两件事发生：

- 内存被分配出来（通过`operator new`）。
- 针对此内存会有一个或者多个构造函数被调用。

当你使用`delete`，也有两件事发生：

- 针对此内存会有一个或者多个析构函数被调用。
- 然后内存才被释放（通过`operator delete`）

`delete`的最大问题：被删除的内存之内究竟有多少对象。

```c++
//来看正确例子

std::string * stirngPtr1 = new std::string;
std::string * stirngPtr2 = new std::stirng[100];

delete stirngPtr1;			//删除一个队形啊
delete [] stringPtr2;			//删除一个由对象组成的数组
```



如果对`stirngPtr1`使用`delete []`。`delete`会读取若干内存并将它解释为数组大小，然后开始多次调用析构函数，浑然不知它所处理的那块内存不但不是个数组，也或许并未持有它准备销毁的对象。



反过来。`delete`导致太少的析构函数被调用。但是对内置类型会发生不明确行为。



所以，使用`new`和`delete`的规则如下：

- 如果调用`new`时使用`[]`，那么在对应`delete`时也要`[]`。
- 如果调用`new`时没有使用`[]`，那么在对应`delete`时就不要用`[]`。



我们来看看如果对类型使用了`typedef`会发生什么事情。

```c++
typedef std::string AddressLines[4]; //每个人的地址是4行，每行是一个stirng

std::string * pal = new AddressLines; //注意，`new AddressLines`返回一个string *, 就像new string[4]一样。

//那么就必须匹配数组的形式
delete pal;//不安全
delete [] pal; //正确
```



==如果你在new表达式中使用[]，必须在相应的delete表达式中也使用[]==

==如果你在new表达式中不使用[]，一定不要在相应的delete表达式中使用[]==



## 以独立语句将newed对象置入智能指针

假设我们有个函数用来揭示处理程序的优先权，另一个函数用来在某动态分配所得的Widget上进行某些带有优先权的处理：

```c++
int priority();
void processWidget(std::tr1::shared_ptr<Widget> pw, int priority);

//调用
processWidget(new Widget, priorty()); //不能通过编译，因为shared_ptr不能进行隐式转换

processWidget(std::tr1::shared_ptr<Widget> (new Widget), priority()); //OK/
```



但是上述的调用还是可能会发生资源泄露。



编译器产出一个调用码时，必须首先核算即将被传递的各个实参。其中第一个参数的调用由两部分组成：

- 执行`new Widget`表达式。
- 调用`tr1::shared_ptr`构造函数。

所以在调用`processWidget`之前，编译器必须创建代码，做一下事情：

- 调用`priority`。
- 执行`new Widget`表达式。
- 调用`tr1::shared_ptr`构造函数。

但是上面的次序除了23是固定的，第一个调用不一定是在第一步执行，也可能在23之间。

如果出现以下调用顺序：

1. 执行`new Widget`表达式。
2. 调用`priority`。
3. 调用`tr1::shared_ptr`构造函数。

如果调用`priority`出现了问题。那么这个时候内存就会发生泄露，因为没办法再执行下面的语句了。而创建的new也没有被delete。



所以应该修改为如下形式：

```c++
std::tr1::shared_ptr<Widget> pw(new Widget); //单独语句内智能指针存储

processWidget(pw,priority());
```



==以独立语句将new ed对象存储于智能指针内。如果不这样做，一旦异常抛出，有可能导致难以察觉的资源泄露==



