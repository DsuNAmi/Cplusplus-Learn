# 智能指针

## 裸指针存在的问题

- 裸指针在声明中并没有指出，裸指针指涉到的是单个对象还是一个数组。
- 裸指针在声明中也米有提示在使用完指涉的对象以后，是否需要析构它。换言之，你从声明中看不出来指针是否拥有其指涉的内容。
- 即使知道需要析构指针所指涉的对象，也可能知道如何析构才是适当的。是应该使用`delete`运算符呢，还是别有它途（例如，可能需要把指针传入一个专门的、用于析构的函数）
- 即使知道了应该使用`delete`运算符，参见理由1，还是会发生到底应该用单个对象形式`delete`，还是数组形式`delete []`的疑问。一旦用错，就会导致未定义行为。
- 即启用够确信，指针拥有其指涉的对象，并且也确知应该如何析构，要保证析构在所有代码路径上都仅执行一次（包括那些异常导致的路径）仍然困难重重。只要少在一条路径上执行，就会导致资源泄露。而如果析构在一条路径上执行了多于一次，则会导致未定义行为。
- 没有什么正规的方式能够检测出指针是否空悬，也就是说，它指涉的内容是否已经不再持有指针本应该指涉的对象。如果一个对象已经被析构了，而某些指针仍然指涉它，就会产生空悬指针。



###### c++11中一共有4种智能指针

- `std::auto_ptr`
- `std::unique_ptr`
- `std::shared_ptr`
- `std::weak_ptr`



# 条款18，使用std::unique_ptr管理具备专属所有权的资源

可以认为在默认情况下，`std::unique_ptr`和裸指针有着相同的尺寸，并且对于大多数的操作，它们都是精确地执行了相同的指令。其实现的是**专属所有权语义**。

一个非空的`std::unique_ptr`总是拥有其所指涉到的资源。

移动一个`std::unique_ptr`会将所有权从源指针移至目标指针（源指针会被置空）。

`std::unique_ptr`不允许复制，因为如果复制了一个该指针，就会得到两个指涉一个资源的该指针，所以两者都认为要析构该资源。

`std::unique_ptr`是一个只移型别。在执行析构操作时，由非空的`std::unique_ptr`析构其资源。



我们来看一个`std::unique_ptr`的常见用法：**在对象继承谱系种作为工厂函数的返回型别**。

```c++
//假设一个投资型别的谱系
class Investment {};

class Stock : public Investment {};

class Bond : public Investment {};

class RealEstate : public Investment {};

这种继承谱系的工厂函数通常会在堆上分配一个对象并且返回一个指涉到它的指针，并当不再需要该对象时，由调用者负责删除。;

//构建工厂模型
template<typename Ts>
std::unique_ptr<Investment> makeInvestment(Ts && params);

//使用
{
    auto pInvestment = makeInvestment( arguments );
} 		//*pInvestment 在这里析构

除了一些提前中断的情况，比如std::abort或者是std::quick_exit，该指针指着的资源一定会被析构。
```



默认地，析构通过`delete`运算符实现，但是在析构过程中`std::unique_ptr`可以被设置为使用自定义析构器：

析构资源时所调用的任意函数（或函数对象，包括那些由lambda表达式产生的）。如果由`makeInvestment`创建的对象不应被直接删除，而是应该先写入一条日志：

```c++
auto delInvmt = [](Investment * pInvestment)
{
    //一个作为自定义析构器的lambda表达式
    makeLogEntry(pInvestment);
    delete pInvestment;
};

template<typename Ts>
std::unique_ptr<Investment, decltype(delInvmt)>
makeInvestment(Ts && params)
{
    std::unique_ptr<Investment, decltype(delInvmt)> pInv(nullptr, delInvmt);
    
    if(/*创建一个Stock对象*/)
    {
        pInv.reset(new Stock(std::forward<Ts>(params)));
    }
    else if(/*创建一个Bond对象*/)
    {
        pInv.reset(new Bond(std::forward<Ts>(params)));
	}
    else
    {
        pInv.reset(new RealEstate(std::forward<Ts>(params)));
    }
    
    return pInv;
}
```



对上述代码的解释：

- `delInvmt`是由`makeInvestment`所返回的对象自定义析构器，所有自定义删除函数都接受一个指涉到欲析构对象的裸指针，然后采取必要措施析构该对象。
- 当要使用自定义析构器时，其型别必须被指定为`std::unique_ptr`第二个实参的型别。也就是`delInvmt`的型别，使用自动推断。
- `makeInvestment`的基本策略是创建一个空的`std::unique_ptr`，使其指涉到适当的型别对象，然后将其返回。
- 将一个裸指针赋值给`std::unique_ptr`的尝试，不会通过编译。因为这回造成从裸指针到智能指针的隐式型别转换。这种隐式型别转换大有问题，因此C++11种的智能指针将这种操作禁止了。这就是为什么需要使用`reset`来指定让`pInv`获取从使用`new`运算符产生的对象。
- 对每一次`new`运算符的调用结果，都使用`std::forward`对传递给`makeInvestment`的实参实施完美转发。
- 自定义析构器接受一个型别为`Investment*`的形参，不管怎么样，都会被作为一个`Investment`对象被删除，所以我们必须要提供一个虚析构函数。

```c++
class Investment
{
    public:
    	...;
    	virtual ~Investment();		//必须要是虚的，且析构函数必须编写
    	...;
};
```



之前说过，在使用默认析构器的前提下，`std::unique_ptr`的尺寸和裸指针相同，但是自定义之后，如果析构器是函数指针，那么一般会增加一到两个字长。如果是函数对象，那么带来的尺寸变化取决于该函数对象中存储了多少状态。**无状态的函数对象（比如无捕获的lambda表达式）不会浪费任何存储尺寸**。



`std::unique_ptr`提供两种形式，一种是单个对象`std::unique_ptr<T>`，另一种是数组`std::unique_ptr<T[]>`。这样区分的结果是，对于`std::unique_ptr`指涉的对象不会产生二义性。



`std::unique_ptr`可以高效地转换成`std::shared_ptr`：

```c++
std::shared_ptr<Investment> sp = makeInvestment( arguments );
```



###### 总结

- `std::unique_ptr`是小巧，高速的，具备只移型别的智能指针，对托管资源实施专属所有权语义。
- 默认的，资源析构采用`delete`运算符实现，但可以指定自定义删除器。有状态的删除器和采用函数指针实现的删除器会增加`std::unique_ptr`型别的对象尺寸。
- 将`std::unique_ptr`转换成`std::shared_ptr`是容易实现的。



# 条款19，使用std::shared_ptr管理具备共享所有权的资源

通过`std::shraed_ptr`这种智能指针访问的对象采用共享所有权来管理其生存期。没有哪个特定的`std::shared_ptr`拥有该对象。取而代之的是，所有指涉到它的`std::shared_ptr`共同协作，确保在不再需要该对象的时刻将其析构。当最后一个指涉到某对象的`std::shared_ptr`不再指涉到它时，该`std::shared_ptr`会析构其指涉到的对象。

其通过访问某资源的引用计数来确定是否自己是最后一个指涉到该资源的。

构造函数计数器+，析构函数计数器-，复制赋值运算符会同时就行两种操作，但是对象不同。



`std::shared_ptr`的一些特点：

- 其尺寸是裸指针的两倍。因为它们内部包含一个指涉到该资源的裸指针，也包含一个指涉到该资源的引用计数的裸指针。
- **引用计数的内存必须动态分配**。从概念上说，引用计数与被指涉到的对象相关联，然而被指涉的对象对此却一无所知。因此它们没有存储引用计数的位置。
- **引用计数的递增和递减必须是原子操作**，因为在不同线程中可能存在并发的读写器。



同样，`std::shared_ptr`也支持自定义析构器。但是用法却有些不同：

```c++
auto loggingDel = [](Widget * pw)
{
    makeLogEntry(pw);
    delete pw;
};

//自定义析构器是其一部分
std::unique_ptr<Widget,decltype(loggingDel)> upw(new Widget, loggingDel);
//并非是其一部分
std::shared_ptr<Widget> spw(new Widget, loggingDel);

//这样的优点就是可以给不同的对象创建不同的自定义析构器
auto customDelete1 = [](Widget * pw){};
auto customDelete2 = [](Widget * PW){};

std::shared_ptr<Widget> pw1(new Widget, customDelete1);
std::shared_ptr<Widget> pw2(new Widget, customDelete2);

std::vector<std::shared_ptr<Widget>> vpw {pw1,pw2};
```



还有一点不同的是，`std::shared_ptr`不会随着自定义析构器的改变而改变其尺寸，其尺寸始终是裸指针的两倍。并不是因为没有使用更多的内存，而是这一部分的内存不属于`std::shared_ptr`。而是每一个`std::shared_ptr`都管理一个控制块：

- 该指针指针总是创建一个控制块。`std::make_shared`会生产出一个用以指涉到的新对象，因此在调用`std::make_shared`的时刻，显然不会有针对该对象的控制块存在。
- 从具备专属所有权的指针（`std::unqiue_ptr, std::auto_ptr`)出发构造一个`std::shard_ptr`时，会创建一个控制块。专属所有权指针不使用控制块，因此不应该存在所指涉到的对象来说不应存在控制块。
- 当`std::shared_ptr`构造函数使用裸指针作为实参来调用时，它会创建一个控制块。如果想从一个已经拥有控制块的对象出发来创建一个`std::shared_ptr`，你大概会传递一个`std::shared_ptr`或`std::weak_ptr`而非裸指针作为构造函数的实参。则不会创造新的控制块。

这些规则会导致一个后果：从同一个裸指针出发来构造不止一个`std::shared_ptr`的话，被指涉到的对象将会有多重的控制块。多重的代码就会有各种多重的操作，我们来看下面一段代码：

```c++
auto pw = new Widget;			//pw是一个裸指针

std::shared_ptr<Widget> spw1(pw, loggingDel);
std::shared_ptr<Widget> spw2(pw, loggingDel);       //为pw创建了两个控制块。

上述代码会引发严格的未定义行为。
```



根据上述的代码：

- 尽可能避免将裸指针传递给一个`std::shared_ptr`的构造函数。常用的替代手法是使用`std::make_shared`。
- 如果必须将一个裸指针传递个`std::shared_ptr`，就直接传递`new`的运算符的结果，而非传递一个裸指针变量。

```c++
std::shared_ptr<Widget> spw1(new Widget, loggingDel);		//直接传递new表达式

std::shard_ptr<Widget> spw2(spw1);			//两个对象使用的是同一个控制块。
```



###### 总结

- `std::shared_ptr`提供方便的手段，实现了任意资源在共享所有权语义下进行声明周期管理的垃圾回收。
- 与`std::unique_ptr`相比，`std::shared_ptr`的尺寸通常是裸指针尺寸的两倍，它还会带来控制块的开销，并要求原子化的引用计数操作。
- 默认的资源析构通过`delete`运算符进行，但同时也支持定制删除器。删除器的型别对`std::shared_ptr`的型别没有影响。
- 避免使用裸指针型别的变量来创建`std::shared_ptr`。



# 条款20，对于类似std::shared_ptr但有可能空悬的指针使用std::weak_ptr

`std::weak_ptr`能够像`std::shared_ptr`那样运作，但又不影响其指涉对象的引用计数。但是这样会存在一个问题，其所指涉的对象有可能已被析构。

这是因为`std::weak_ptr`并不是一种独立的指针指针，而是`std::shared_ptr`的一种扩充。

这种关系在指针生成的时刻就已存在。`std::weak_ptr`一般是通过`std::shared_ptr`来创建的。当使用`std::shraed_ptr`完成初始化`std::weak_ptr`的时刻，两者就指涉到了相同位置，但`std::weak_ptr`并不影响所指涉到的对象的引用计数：

```c++
auto spw = std::make_shared<Widget>();	//spw构造完成后，其引用计数置为1

std::weak_ptr<Widget> wpw(spw);			//指涉同一个Widget，引用计数保持1

spw = nullptr;							//引用计数为0，Widget被析构，wpw空悬


if(wpw.expired()) ;			//如果其空悬

但是一般来说，我想要判断wpw是否有效，如果有效才访问资源。;但是使用std::weak_ptr写不出这样的代码。;
```



我们需要一个原子操作来完成`std::weak_ptr`是否失效的检验，以及在未失效的条件下提供对所指涉到的对象访问。`std::weak_ptr::lock`，它返回一个`std::shared_ptr`，如果失效了，则`std::shared_ptr`为空。

```c++
std::shared_ptr<Widget> spw1 = wpw.lock();			//wpw失效，则spw1为空

auto spw2 = wpw.lock();								//使用auto
```



另一种是使用`std::weak_ptr`作为实参来构造`std::shared_ptr`，如果失效，抛出异常：

```c++
std::shared_ptr<Widget> spw3(wpw);			//失效，抛出异常
```





我们来看一个用例：观察设计者模式。

该模式的主要组件是主题（可以改变状态的对象）和观察者（对象状态发生改变后通知的对象）。

在多数实现中，每个主题包含了一个数据成员，该成员持有指涉到其观察者的指针，这使得主题能够很容易在其发生状态改变时发出通知。主题不会控制其观察者的生存期，但需要确认的话，当一个观察者被析构以后，主题不会去访问它。一种合理的设计就是让每个主题持有一个容器来放置指涉到其观察者的`std::weak_ptr`，以便在主题使用某个指针之前，能够确定它是否空悬。



我们最后看一个例子：考虑一个包含A、B、C三个对象的数据结构，A和C共享B的所有权，因此各自持有一个指涉到B的`std::shared_ptr`，为了使用方便，假设有一个指针从B指回A，那么该指针应该使用何种型别？

- 裸指针：在此情况下，如果A被析构，而C仍然指着B，B保存着A的空悬指针，但是B无法检测出来，所有B可能回去访问这个空悬指针，就会产生未定义行为。
- `std::shared_ptr`：这种设计中，A和B相互保存着指涉对方的`std::shared_ptr`，这种`std::shared_ptr`环路阻止了A和B被析构。即使其他的程序不能再访问A和B，两者也会保持彼此的引用计数一样。这样，A和B发生了内存泄漏，但是程序已经访问不到A和B，资源也得不到回收。
- `std::weak_ptr`：假如A被析构，那么B的回指针将会空悬，但是B能够检测到这一点。尽管A和B将指涉到彼此，但是B持有的指针不会影响A的引用计数，因此当`std::shared_ptr`不再指涉到A时，不会阻止A被析构。

###### 总结

- 使用`std::weak_ptr`来替代可能空悬的`std::shared_ptr`。
- `std::weak_ptr`可能的用武之地包括缓存，观察者列表，以及避免`std::shared_ptr`环路。

# 条款21，优先选用std::make_unique和std::make_shared，而非直接使用new

我们来看一个基础版本的`std::make_unique`

```c++
template<typename T, typename Ts>
std::unique_ptr<T> make_unique(Ts&& params)
{
    return std::unique_ptr<T>(new T(std::forward<Ts>(params)));
}
```

`make_unique`仅仅是将其形参像待创建对象的构造函数作了一次完美转发。

`make`系列函数会把一个任意实参集合完美转发给动态分配内存的对象的构造函数。



我们来看一个区别：

```c++
auto upw1(std::make_unique<Widget>());			//使用make函数
std::unique_ptr<Widget> upw2(new Widget);		//不适用
auto spw1(std::make_shared(Widget()));
std::shared_ptr<Widget> spw2(new Widget);
```



第一个优点是能够减少代码冗余。



第二个原因是和异常安全有关。假设我们有一个函数依据某种优先级来处理一个`Widget`对象。

```c++
void processWidget(std::shraed_ptr<Widget> spw, int priority);

//有一个函数计算优先级
int computerPriority();

//
processWidget(std::shared_ptr<Widget>(new Widget), computerPriority());		
//该问题已经在Effiective C++中阐述，一共有3步

//修改为如下代码不会发生资源泄露
processWidget(std::make_shared<Widget>(),computerPriority());

//因为在代码在执行试，不会出现3步，而是两步。
//但是如果要使用自定义析构器，只能使用new来创建对象，那么应该怎么修改代码呢
//1.分成两句话
auto  cusDel = [](Widget * pw){};
std::shared_ptr<Widget> spw(new Widget, cusDel);
processWidget(spw, computerPriority()); //但是spw是一个左值，去构造之前要复制一次
//修改
processWidget(std::move(spw),computerPriority());
```



第三个原因是性能的提升。

```c++
std::shared_ptr<Widget>(new Widget);
```



这段代码会引发两次内存分配，一次是`new`分配，第二次是将`Widget`分配到`std::shared_ptr`上的一次内存分配。

```c++
std::make_shared<Widget>();
```



只会产生一次内存分配。因为`std::make_shared`会分配单块内存，即保存`Widget`对象又保存与其相关联的控制块。这种优化减小了程序的静态尺寸。



但是，**所有的make系列函数都不允许使用自定义析构器**。此时只能使用`new`。

第二个限制是初始化时的限制：**假如需要使用大括号初始化物来创建指涉到对象的指针，就必须使用new表达式了**，但是还是有别的办法可以使用`std::make`系列函数来大括号初始化

```c++
auto upv = std::make_unique<std::vector<int>>(10,20);		//10个值为20的元素
auto spv = std::make_shared<std::vector<int>>(10,20);		//10个值为20的元素


//创建std::initializer_list型别的对象
auto initList = {10,20};
//利用std::initializer_list型别的构造函数创建std::vector
auto spv = std::make_shared<std::vector<int>>(initList);
//两个元素，一个时是10，另一个是20
```



###### 总结

- 相比于直接使用`new`表达式，`make`系列函数消除了重复代码、改进了异常安全性，提高了代码效率。
- 不适用于`make`系列函数的场景包括需要定制删除器，以及期望直接传递大括号初始化物。
- 对于`std::shared_ptr`，不建议使用`make`系列函数的额外场景包括：（1）自定义内存管理的类。（2）内存紧张的系统，非常大的对象，以及存在比指涉到相同对象的`std::shared_ptr`生存期更久的`std::weak_ptr`。



# 条款22，使用Pimpl习惯用法时，将特殊成员函数定义放到实现文件中

这种技巧就是把某类的数据成员用一个指涉到某实现类的指针替代，然后把原来在主类中的数据成员放置到实现类中，并通过指针间接访问这些数据成员：

```c++
//Widget.h
class Widget
{
    public:
    	Widget();
    	...;
    private:
		std::string name;
    	std::vector<double> data;
    	Gadget g1, g2, g3;
};

//客户必须包含#include<string>, #include<vector>, #inlclude "Gadget.h"才能通过编译
使用Pimpl;
class Widget
{
    public:
    	Widget();
    	~Widget();
    private:
		struct Impl;
    	Impl * pImpl
}

//Widget.cpp
#include "widget.h"
#include "gadget.h"

#include <string>
#include <vector>

struct Widget::Impl
{
  	std:string name;
    std::vector<double> data;
    Gadget g1, g2, g3;
};

Widget::Widget()
    : pImpl(new Impl)
{
    
}

Widget::~Widget()
{
    delete pImpl;
}
```



以上是C++98的写法，我们来看看C++11的写法：

```c++
//widget.h文件
class Widget
{
    public:
    	Widget();
    private:
		struct Impl;
    	std::unique<Impl> pImpl;
}

//Widget.cpp
#include "widget.h"
#include "gadget.h"

#include <string>
#include <vector>

struct Widget::Impl
{
  	std:string name;
    std::vector<double> data;
    Gadget g1, g2, g3;
};

Widget::Widget()
    : pImpl(std::make_unique<Impl>())
{
    
}
```



但是这段代码仍然存在问题：

```c++
#include "widget.h"

Widget w;			//错误！！！
```



错误原因可能是**在非完整型别上实施了sizeof或delete运算符**

该问题是由w被析构时（比如离开了作用域）所生成的代码引起的。因为我们没有写析构函数，但是编译期会为哦我们写一个析构函数。在该析构函数内，编译期会插入代码来调用`Impl`，其是一个`std::unique_ptr<Widget::Impl>`的对象，即一个使用了默认析构器的`std::unique_ptr`。析构时，使用`delete`运算符，但是在使用`delete`运算符之前，C++11会使用`static_assert`来确保裸指针未指涉到非完整型别。这样在产生析构函数代码时，就会发生一个错误的`static_assert`。

为了解决这个问题，只需要保证在生成析构函数前，保证`Widget::Impl`是一个完整型别。方法就是自己声明一个析构函数，并且定义它，在定义的后面加上`default`。只要型别的定义可以被看到，那它就是完整的。



同时Pimpl还支持移动操作，但是我们先来看一个例子

```c++
//widget.h文件
class Widget
{
    public:
    	Widget();
    	~Widget();
    	Widget(Widget && rhs) = default;				//正确
    	Widget& operator=(Widget && rhs) = default;		//错误,理由同上
    private:
		struct Impl;
    	std::unique<Impl> pImpl;
};
```



解决办法也是把定义写在实现文件里，然后后面加`default`。声明的时候不用加。



###### 总结

- `Pimpl`惯用法通过降低类的客户和类实现者之间的依赖性，减少了构建次数。
- 对于采用`std::unique_ptr`来实现的`pImpl`指针，须在类的头文件中声明特种函数，但在实现文件中实现它们。
- 上述结论仅仅适用于`std::unique_ptr`，并不适用`std::shared_ptr`。





