# 右值引用、移动语义和完美转发

**移动语义**使得编译器得以使用不那么昂贵的移动操作，来替换昂贵的复制操作。同复制构造函数、复制赋值运算符给予人们控制对象复制的具体意义的能力一样，移动构造函数和移动复制赋值运算符也给予人们控制对象移动语义的能力。移动语义也使得创建只移型别对象成为可能，这些型别包括`std::unique_ptr,std::futrue,std::thread`。



**完美转发**使得人们可以撰写接受任意实参的函数模板，并将其转发到其他函数，目标函数会接受到与转化函数所接受的完全相同的实参。



一定要铭记一点：**形参总是左值，即使其型别是右值引用。**



### 左值和右值

C++表达式要么是左值要么是右值。当一个对象被用作右值时用的是对象的值；当被用作左值时，用的是对象的存储空间。左值可以当右值使用，但是右值不可以当左值使用。

**左值**：可以看作是存储在内存中的，有明确存储地址的（可寻址）的数据。

**右值**：指的是可以提供数据值的数据（不一定可以寻址，比如常量是存储于寄存器中的数据）。



简单点说，左值以变量的形式存在，指向内存，生命周期比较长，我们可以对左值进行各种操作。

而右值通常以常量的形式存在，是一个临时值，不能被程序的其他部分访问，生命周期很短。



```c++
int x = 8;

x是左值、8是右值
```



> 1. 可以在赋值号左侧的表达式是左值；只能位于赋值号右侧的表达式就是右值
> 2. 有名称的，可以获取到存储地址的表达式是左值，反之是右值。



###### 左值引用

我们知道，引用是一个变量的别名，它指向的是已经存在的其他变量的内存地址。

```c++
int x = 8;				//x为左值，8为右值。
int &y = x;				//y相当于x的别名，y指向的是x的地址。y也称左值引用，因为y有指向的内存。
y = 88;					//修改y和x的值为88

int & ref = 666;		//错误，没有指向666的内存。
const int & y = 666;.	//合法，此时编译期会创建一个隐藏的变量（左值），且y的值不能修改。
```



###### 右值引用

C++中引用了右值引用，表示一个没有名称的临时对象即右值，可以修改这个临时对象的值；

```c++
int && ref = 666;			//合法
ref = 88;					//合法
```

但是右值引用不能约束到左值上

```c++
int x = 888;
int && ref = x;			//错误。
```





# 条款23，理解std::move和std::forward

须知，两个函数的行为和其本身字面的含义并不相同。这两者在运行期都无所作为，甚至一行代码都不会生成。

两者都是仅仅执行强制型别转换的函数。

`std::move`无条件地将实参强制转换成右值，`std::forward`则仅在某个特定条件满足时才执行同一个强制转换。



来看一个`std::move`的实例实现：

```c++
//c++11
template<typename T>
typename remove_reference<T>::type&& move(T && param)
{
    using ReturnType = typename remove_reference<T>::type&&;
    return static_cast<ReturnType>(param);
}

//c++14
template<typename T>
decltype(auto) move(T && param)
{
    using ReturnType = remove_reference_t<T>&&;
    return static_cast<ReturnType>(param);
}

std::move的形参是指涉到一个对象的引用（万能引用），它的返回是指涉到同一个对象的引用。
```

作用：

> 将实参强制转换成右值。



当然右值是可以实施移动的，所以在一个对象上实施了`std::move`，就是告诉该编译器该对象具备可移动的条件。其简化了对象是否可以移动的表述。



```c++
class Annotation
{
    public:
    	explicit Annotation(const std::string text) : value(std::move(text)); {}
    	//并不会“移动”入text,而是复制进text
    private:
    	std::string value;
};

//原因
class string
{
    public:
    	...;
    	string(const string & rhs);		//复制构造函数
    	string(string && rhs);			//移动构造函数，并没有const std::string作为右值的形参，所以并不会移动，而是使用复制构造函数进行复制。
};
```

> 1. 如果像取得某个对象执行移动操作的能力，则不要将其声明成常量，因为针对常量对象执行的移动操作将一声不响地变成复制操作。
> 2. `std::move`不仅不实际移动任何东西，甚至不保证经过其强制型别转换后的对象具备可移动的能力。唯一可以确定的是，该结果是一个右值。



`std::forward`是一个有条件的强制型别转换。回忆一下`std::forward`的使用场景。其中最常见的，就是某个函数模板取用了万能引用型别为形参，随后将其传递给另一个参数：

```c++
void process(const Widget & lvalArg);           //处理左值
void process(Widget && rvalArg);                //处理右值


template<typename T>
void logAndProcess(T && param)
{
    auto now = std::chrono::system_clock::now();

    process(std::forward<T>(param));
}

//考虑两种调用情形，一种向其传入左值，一种向其传入右值
Widget w;
LogAndprocess(w);                     //左值
LogAndprocess(std::move(w));          //右值

我们会期望，当传入的是一个左值，则该左值可以在被传递给process函数仍被当作一个左值；而如果传入的是一个右值，则调用的是process取用右值型别的那个重载版本;
但是，所有的函数形参皆为左值，param也不例外。所以LogAndprocess里的process调用就都是左值的那个版本。;
那么现在就需要一种机制，当且仅当用来初始化param的实参是个右值的情况下，把param强制转换成右值。;
所以std::forward仅当其实参是使用右值完成初始化时，它才会执行向右值型别的强制型别转换。
```



我们来看一个例子：

考虑一个类，我们打算用它进行移动构造函数的调用次数的跟踪。所需要的全部设施，就是一个随着移动构造操作而递增的静态计数器罢了。假设类中的唯一非静态数据成员是个`std::string`型别的对象，以下是个实现移动构造函数的经典途径

```c++
//使用std::move
class Widget
{
    private:
        static std::size_t moveCtroCalls;
        std::string s;

    public:
        Widget(Widget && rhs) : s(std::move(rhs.s)) {++moveCtroCalls;}

};

//使用std::forward
class Widget
{
    private:
        static std::size_t moveCtroCalls;
        std::string s;

    public:
        Widget(Widget && rhs) : s(std::forward<std::string>(rhs.s)) {++moveCtroCalls;}
};


```



- `std::move`只需要取用一个函数参数，而`std::forward`则既须取用一个函数实参，又须取用一个模板型别参数。
- 传递给`std::forward`的实参型别应当是一个非引用型别，因为习惯上它编码的所传递的实参应该是一个右值。



###### 总结

- `std::move`实施的是无条件向右值型别的转换。就其本身而言，它不会执行移动操作。
- 仅当传入的实参被绑定到右值时，`std::foward`才针对该实参实施向右值型别的强制抓换。
- 在运行期，`std::move`和`std::forward`都不会做任何操作。



# 条款24，区分万能引用和右值引用

```c++
void f(Widget && param);			// 右值引用

Widget && var1 = Widget();			//右值引用

auto && var2 = var1;				//非右值引用

template<typename T>
void f(std::vector<T>&& param);		//右值引用

template<typename T>
void f(T&& param);					//非右值引用
```



`T&&`有两种不同的含义：

- 右值引用，它们仅仅会绑定到右值。
- 既可以是右值引用，又可以是左值引用，二者居一。带有这种含义的引用在代码中形如右值引用，但它们可以像左值引用一样运作。它们也可以绑定到`const`对象和非`const`对象，以及`volatile`对象和非`volatile`对象。它们几乎可以绑定到万事万物，所以称之为**万能引用**。

万能引用会出现在两种情况下：

```c++
//第一种情况，函数模板的形参
template<typename T>
void f(T && param);			

//第一个场景是auto声明
auto&& var2 = var1;
```

> 这两个场景的共同之处，就是它们都涉及型别推导。
>
> 如果你看到了`T&&`，缺没有涉及的型别推导，那么你看到是一个右值引用：
>
> `void f(Widget && param)`
>
> `Widget && var1 = Widget()`



因为万能引用是一个引用，所以初始化是必须的。万能引用的初始化物会决定它代表的是个左值还是右值引用；如果初始化物是右值，万能引用就会对应到一个右值引用；如果初始化物是一个左值，那么就会对应到一个左值引用。

```c++
template<typename T>
void f(T&& param);

Widget w;			//左值被传递给f，param的型别是Widget&

f(std::move(w));		//右值被传递给f
```





以下情况还是右值引用：

```c++
template<typename T>
void f(std::vector<T>&& param);		//param是个右值引用

std::vector<int> v;					//错误！不能给右值引用绑定一个左值
f(v);								//

template<typename T>
void f(const T&& param);			//param是个右值引用
```



###### push_back和emplace_back

如果你在模板内看到一个函数形参的型别写作`T&&`，你可能会想当然地认为它肯定是一个万能引用，但是**位于模板内**并不一定能保证一定涉及型别推导。

```c++
template<class T, class Allocator = allocator<T>>
class vector{
  	public:
    	void push_back(T&& x);
};

因为push_back作为vector的一部分，如果不存在特定的vector实例，则也无从存在。;
即给定：;
std::vector<Widget> v;
会导致模板具现化为如下实例：;
class vector<Widget, allocator<Widget>>{
	public:
    	void push_back(Widget && x);			//右值引用
};
```



但是于其类似的`emplace_back()`却实实在在地涉及型别推导

```c++
template<class T, class Allocator = allocator<T>>
class vector{
  	public:
    	template <class...Args>
        void emplace_back(Args&&...args);
};
其中型别形参Args独立于vector的型别参数T，所以，Args必须每次emplace_back被调用时进行推导。
```



###### 总结

- 如果函数模板形参具备`T&&`型别，并且T的型别系推导而来，或如果对象使用`auto&&`声明其型别时，则该形参或对象就是个万能引用。
- 如果型别声明并不精确地具备`type&&`的形式，或者型别推导并未发生，则`type&&`就代表右值引用。
- 若采用右值来初始化万能引用，就会得到一个右值引用。若采用左值来初始化万能引用，就会得到一个左值引用。



# 条款25，针对右值引用实施std::move，针对万能应用实施std::forward

右值引用仅会绑定到那些可移动的对象上。如果形参型别为右值引用，它绑定的对象可供移动。

为了是函数能够利用对象的右值性，可以使用`std::move`。

```c++
class Widget
{
    private:
        std::string name;
        std::shared_ptr<int> p;
    public:
        Widget(Widget && rhs) : name(std::move(rhs.name)), p(std::move(rhs.p)) {}
        

};
```



为了能够使万能引用在使用右值初始化时转换为右值型别，可以使用`std::forward`

```c++
class Widget
{
    public:
    template<typename T>
    void setName(T && newName)
    {
        name = std::forward<T>(newName);
    }
};
```



简而言之，当转发右值引用给其他函数时，应当对其实施向右值的无条件强制型别转换（`std::move`），因为它们一定绑定到右值；而当转发万能引用时，应当对其实施向右值的有条件强制型别转换（`std::forward`），因为它们不一定绑定到右值。



我们来看一个用反了的例子：

```c++
class Widget
{
    public:
        template<typename T>
        void setName(T && newName)
        {
            name = std::move(newName);
        }
    private:
        std::string name;
        std::shared_ptr<int> P;
};

std::string getWidgetName();		//工厂函数

Widget w;

auto n = getWidgetName();			//n是一个局部变量

w.setName(n);						//将n移入了w

```

> 因为n是一个局部变量，当n被强制赋予name的时候，`setName`函数结束之后，局部变量会消失，n的值会变成一个不确定的值。



有些情况下，要在单一函数内将某个对象不止一次地绑定到右值引用或者万能引用，而且你想保证完成对该对象的其他所有操作之前，其值不被移走，在这种情况下，你就得仅在最后一次使用该引用时，对其实施`std::move`或者是`std::forward`

```c++
template<typename T>
void setSignText(T && text)
{
    sign.setText(text);			//使用text但是不改变其值
    
    auto now = std::chrono::system_clock::now();		//取得当前时间
    
    signHistory.add(now, std::forward<T>());			//有条件的转换
}
```



在按值返回的函数中，如果返回的是绑定到一个右值引用或一个万能引用的对象，则当你返回该引用时，应该对其实施`std::move`和`std::forward`。

```c++
Matrix operator+(Matrix && lhs, const Matrix & rhs)
{
    lhs += rhs;
    return std::move(lhs);			//将lhs移入返回值
}

Matrix operator+(Matrix && lhs, const Matrix & rhs)
{
    lhs += rhs;
    return lhs;						//将lhs复制进返回值
}
```

> `lhs`是左值这件事会强迫编译器将其复制入返回值的存储位置。



```c++
template<typename T>
Fraction redeceAndCopy(T && frac)
{
    frac.reduce();
    return std::forward<T>(frac);			//对于右值，是移入返回值
    										//对于左值，是复制入返回值
}
```

> 如果原始对象是一个右值，它的值应当被移动到返回值上。但如果原始对象是一个左值，那么就必须创建出实实在在的副本。如果省去对`std::forward`的调用，则`frac`会无条件地复制入到`reduceAndCopy`的返回值。



但是我们来看一个例子：

```c++
//复制情况
Widget makeWidget()
{
    Widget w;
    return w;				//w复制入返回值
}

//假优化
Widget makeWidget()
{
    Widget w;
    return std::move(w);			//不能这样做，移入值
}
```

> 根本原因在于，`makeWidget`的复制版本可以通过直接在为函数返回值分配的内存上创建局部变量`w`来避免复制之，这就是**返回值优化（return value optimization, RVO）**。
>
> 但是编译期若要在一个案值返回的函数里省略对局部对象的复制，则需要满足两个前提条件：（1）局部对象型别和函数返回值型别相同。（2）返回的就是局部对象本身。
>
> 对于上述的代码，任何合格的C++编译器都会应用**RVO**来避免复制`w`。



###### 总结

- 针对右值引用的最后一次使用实施`std::move`，针对万能引用的最后一次使用实施`std::froward`。
- 作为按值返回函数的右值引用和万能引用，依上一条所述采取相同行为。
- 若局部对象可能适用于返回值优化，则请勿针对其实施`std::move`或`std::forward`。



# 条款26，避免依万能引用型别进行重载

假定你需要撰写一个函数，取用一个名字作为形参，然后记录下当前日期和时间，再把该名字添加到一个全局数据结构中。

```c++
std::multiset<std::string> name;

void logAndAdd(const std::string & name)
{
    auto now = std::chrono::system_clock::now();
    
    log(now,"logAndAdd");
    
    names.emplace(name);
}

std::string petName("Darla");				

logAndAdd(petName);							//传递左值std::string

logAndAdd(std::string("Persephone"));		//传递右值std::string

logAndAdd("Patty Dog");						//传递字符串字面量
```

> 第二个和第三个调用，完全可以避免复制情况，但是还是出付出了复制的代价

```c++
//修改
template<typename T>
void logAndAdd(T&& name)
{
    auto now = std::chrono::system_clock::now();
    log(now,"logAndAdd");
    names.emplace(std::forward<T>(name));
}

std::string petName("Darla");

logAndAdd(petName);				//同之前

logAndAdd(std::string("Persephone"));			//对右值实施移动而非复制

logAndAdd("Patty Dog");							//在multiset中直接构造对象，而不是复制一个临时对象
```



但是如果客户只能通过索引访问到名字

```c++
std::string nameFromIdx(int idx);

void logAndAdd(int idx)
{
    auto now = std::chorno::system_clock::now();
    
    log(now, "logAndAdd");
    
    names.emplace(std::forward<T>(nameFromIdx(idx)));
}

logAndAdd(22);			//没问题

但是如果客户使用short型变量;

short nameIdx;
logAndAdd(nameIdx);			//错误！
```

> 这是因为short用到的是那个模板的版本，而不是`int`的版本。



形参为万能引用的函数，是C++中最贪婪的。它们会在具先过程中，和几乎任何实参型别都会产生精确匹配。

这就是为什么把重载和万能引用结合起来是愚蠢的。



###### 总结

- 把万能引用作为重载候选型别，几乎总会让该重载版本在史料未及的情况下被调用到
- 完美转发构造函数的问题尤其严重，因为对于非常量的左值型别而言，它们一般都会形成相对于复制构造函数的更佳匹配，并且它们还会劫持派生类中对基类的复制和移动构造函数的调用



# 条款27，熟悉依万能引用型别进行重载的替代方案

### 舍弃重载

以`logAndAdd`的两个重载版本为例，就可以分成`logAndAddName`和`logAndAddNameIdx`。

### 传递const T& 型别的形参

这种方法的缺点就是舍弃了高效率。

### 传值

一种经常能够提升性能，却不用增加一点复杂性的方法，就是把传递的形参从引用型别替换为值型别。

当你知道肯定需要复制形参，考虑按值传递对象。

```c++
class Person
{
  	public:
    	explicit Person(std::string n) : name(std::move(n)) {}
    	explicit Person(int idx) : name(nameFromIdx(idx)) {}
    private:
    	std::string name;
};
```

> 由于`std::string`型别并没有只接受单个整型形参的构造函数，所有`int`或者类`int`型别的实参都会汇集到接受`int`型别的那个构造函数重载版本的调用。类似地，所有`std::string`型别的实参，都会到另一个满足条件的构造函数。



### 标签分派

无论是传递左值常量还是传值，都不支持完美转发。如果使用万能引用的动机就是为了完美转发，那就只能采用万能引用。之前说过，万能引用会吸收别的类型的情况，存在精确匹配。

但是，如果万能引用仅是形参列表的一部分，该列表中还有其他非万能引用型别的形参的话，那么只要该非万能引用形参具备充分差的匹配能力，则它就足以将这个带有万能引用形参的重载版本踢出局。这个想法就是**标签分派**



核心思路就是：**重新实现之前的logAndAdd，把她委托给另外两个函数，一个接受整型值，另一个接受其他所有型别。而logAndAdd本身则接受所有的实参**

```c++
template<typename T>
void logAndAdd(T && name)
{
    logAndAddImpl(
        std::forward<T>(name),
    	std::is_integral<typename std::remove_reference<T>::type>()
        );
}

template<typename T>
void logAndAddImpl(T && name, std::false_type)		//对非整形实施，因为是假
{
    auto now = std::chrono::system_clock::now();
    log(now, "logAndAdd");
    names.emplace(std::forward<T>(name));
}

std::string nameFormIdx(int idx);
void logAndAddImpl(int idx, std::true_type)
{
    logAndAdd(nameFromIdx(idx));
}

通过让logAndAddImpl按索引查找对应名字，然后传递给logAndAdd（在那里，它会经由std::forward转发到另一个logAndAddImpl重载版本），就可以避免两个重载版本都放入记录日志的代码。
```

> 型别`std::false_type`和`std::true_type`就是所谓的标签。这些形参甚至没有名字，它们在运行期不起任何作用。这样设计的结果是，只有标签值才决定了调用的是哪个重载版本。



### 对接受万能引用的模板施加限制

上一个提到的方法能够发挥左右的关键在于，存在一个单版本函数作为客户端API

但是对于`Person`类的完美转发构造函数的那个问题，编译器可能会自行生成复制和构造函数，所以如果仅仅撰写一个构造函数，然后在其中运用标签分派，那么有些针对构造函数调用就可能会由编译器生成的构造函数接手处理，从而绕过了标签分派系统。

但是实际上的问题是：**在于编译器生成的函数并不能保证一定会绕过标签分派设计**。

`std::enable_if`可以强制编译器表现出来的行为如同特点的模板不存在一般。这样的模板称为禁用的。默认地，所有模板都是启用的。可是，实施了`std::enable_if`的模板只会在满足`std::enable_if`指定的条件的前提下才会启用。

```c++
class Person
{
    public:
    	template<typename T,
    			 typename = typename std::enable_if<condition>::type>
        explicit Person(T&& n);
};
```

> 我们想指定的条件是，`T`不是`Person`型别，即，仅当`T`是`Person`以外的型别时，才启用该模板构造函数。
>
> `!std::is_same<Person,T>::value`。
>
> 因为`Person`和`Person&`不同，但是万能引用会推导Person和Person&
>
> 最后的表达式应该是
>
> `!std::is_same<Person, typename std::decay<T>::type>::value`



最后的结果是

```c++
class Perosn
{
    public:
    	template<
    		typename T,
    		typename = std::enable_if_t<
                !std::is_base_of<Person, std::decay_t<T>>::value
                &&
                !std::is_integral<std::remove_reference_t<T>::value
              >
         >
         //接受std::string型别以及可以强制转型到std::string的实参型别的构造函数
         explicit Person(T&& n) : name(std::forward<T>(n)) {...;}
    	 //接受整型实参的构造函数
    	 explicit Person(int idx) : name(nameFromIdx(idx)) {...;}
    
    private:
    	std::string name;
                    
};
```



###### 总结

- 如果不使用万能引用和重载的组合，则替代方案包括使用批次不同的函数名字，传递`const T&`型别的形参，传值和标签分派。
- 经由`std::enbale_if`对模板施加限制，就可以将万能引用和重载一起使用，不过这种技术控制了编译器可以调用到接受万能引用的重载版本的条件。
- 万能引用形参通常在性能方面具备优势，但在易用性方面一般会有劣势。



# 条款28，理解引用折叠

```c++
template<typename T>
void func(T&& param);

模板形参T的推到结果型别中，会把传给param的实参是左值还是右值的信息给编码进去。
```



编码机制是直截了当的：**如果传递的实参是个左值，T的推导结果就是个左值引用型别；如果传递的是一个右值，T的推导结果就是个非引用型别**



```c++
Widget widgetFactory();			//返回右值的函数

Widget w;						//变量（左值）

func(w);						//调用func并传入左值，T的推导结果型别为Widget&，

func(widgetFactory());			//调用func并传入左值，T的推导解锁型别为Widget
```



在C++中，引用的引用是非法的：

```c++
int x;

auto && rx = x;				//错误！
```



但是可以存在引用折叠规则：

```c++
template<typename T>
void func(T&& param);

func(w);->void func(Widget& && param);->void func(Widget & param);
```



**如果任一引用为左值引用，那么结果为左值引用。否则（即两个皆为右值引用），结果为右值引用。**

应用折叠是使`std::forward`得以运作的关键：

```c++
template<typename T>
void f(T&& fParam)
{
    ...;			//完成一些操作
    someFunc(std::forward<T>(fParam));			//将fParam转发至someFunc
}
```

> 由于`fParam`是个万能引用，我们就知道，传递给`f`的实参（即用以初始化fParam的表达式）是左值还是右值的信息会被编码到型别`T`中。`std::forward`的任务是，当且仅当编码`T`中的信息表明传递给实参是个右值，即`T`的推导结果型别是个非引用型别时，对`fParam`实施到右值的强制型别转换。



来看具体是如何实现的

```c++
template<typename T>
T && forward(typename remove_reference<T>::type& param)
{
    return static_cast<T&&>(param);
}

假设传递给函数f的实参的型别是个左值Widget，则T会被推导为Widget&型别，然后对std::forward的调用就会实例化为std::forward<Widget&>.而将Widget&插入其就会产生如下结果：;
Widget& && forward(typename remove_reference<Widget&>::type& param)
{
    return static_cast<Widget& &&>(param);
}
由于型别特征remove_reference<Widget&>::type的产生结果是Widget型别，所以：;
Widget & forward(Widget & param)
{
    return static_cast<Widget&>(param);
}
所以，当左值实参被传递给函数模板f时，实例化结果是：接受一个左值引用，并返回一个左值引用。
```



```c++
template<typename T>
T && forward(typename remove_reference<T>::type& param)
{
    return static_cast<T&&>(param);
}

假设传递给f的实参是右值Widget型别。所以：;
Widget && forward(Widget & param)
{
    return static_cast<Widget &&>(param);
}

所以，传递给函数f的右值实参会作为右值转发到someFunc函数。
```



引用折叠出现的语境有四种。

- 模板实例化。
- 是`auto`变量的型别生成。
- 使用`typedef`
- 别名声明

###### 总结

- 引用折叠会在四种语境中发生：模板实例化、`auto`型别生成，创建和运用`typedef`和别名声明，以及`decltype`
- 当编译器在引用折叠的语境下生成引用的引用时，结果会变成单个引用。如果原始的引用中有任一引用为左值引用，则结果为左值引用。否则，结果为右值引用。
- 万能引用就是在型别推导的过程中会区别左值和右值，以及会发生引用折叠语境中的引用。



# 条款29，假定移动不存在，成本高，未使用

```c++
std::vector<Widget> vw1;			//将数据放入vw1

//移动vw1入vw2
//完成执行仅须常数时间
//仅仅是包含在vw1和vw2中的指针被修改了
auto vw2 = std::move(vw1);
```



但是`std::array`型别的对象则缺少这样一个指针，因为其内容数据是直接存储在对象内的：

```c++
std::array<Widget, 10000> aw1;
//将数据放入aw1

//移动aw1入aw2
//完成执行需要线性时间
//需要把aw1中的所有元素移动入aw2
auto aw2 = std::move(aw1);
```



在这几个场景中，C++的移动语义不会给你带来什么好处：

- **没有移动操作**：带移动的对象未能提供移动操作。因此，移动请求就变成了复制请求。
- **移动未能更快**：待移动的对象虽然有移动操作，但并不比其复制操作更快。
- **移动不可用**：移动本可以发生的语境下，要求移动操作不可发射异常，但该操作未加上`noexcept`声明。

###### 总结

- 假定移动操作不存在、成本高、未使用。
- 对于那些型别或对于移动语义支持情况已知的代码，则无需作以上假定。



# 条款30，熟悉完美转发的失败情形

**转发**的含义不过是一个函数把自己的形参传递给另一个函数而已。其目的是**为了让第二个函数（转发目的函数）接受第一个函数（转发发起函数）所接受的同一对象。**

**完美转发**的含义是我们不仅转发对象，还转发其显著特征：型别、是左值还是右值，以及是否带有`const`或`volation`。



假设有某函数`f`，然后我们打算撰写一个函数将`f`作为转发目标。

```c++
template<typename T>
void fwd(T && param)
{
    f(std::forward<T>(param));
}

给定目标函数f和转发函数fwd，当以某特定实参调用f会执行某操作，而用同一实参调用fwd会执行不同的操作，则称完美转发失败。;
f( experssion );
fwd( experssion );
```



有若干种实参会导致该完美转发失败。



###### 大括号初始化物

```C++
假设f声明如下：;
void f(const std::vector<int> & v);

f({1,2,3});				//可以通过编译，隐式转换

fwd({1,2,3});			//错误

问题在于向未声明std::initializer_list型别的函数模板形参传递了大括号初始化物，因为这样的语境按规定，用标准委员会的行话说，叫“非推导语境”。由于fwd的形参未声明为std::initializer_list，编译器就会被禁止在fwd的调用过程中从表达式{1,2,3}出发来推导型别。;


解决方法是先声明一个局部变量;
auto il = {1,2,3};
fwd(il);
```



完美转发会在下面两个条件中的任何一个成立时失败：

- **编译器无法为一个或多个fwd的形参推导出型别结果**。
- **编译器为一个或多个fwd的形参推导出了“错误的”型别结果**。这里的错误，既可以指fwd根据型别推导结果的实例化无法通过编译，也可以指以fwd推导而得得型别调用f与直接传递给fwd的实参调用不一致。



###### 0和NULL用作空指针

**0和NULL都不能用作空指针以进行完美转发**，解决办法是传递nullptr。



###### 仅有声明的整型static const成员变量

有这么一个普适的规定：**不需要给出类中的整型static const成员变量的定义，仅需声明之**。因为编译器会根据这些成员的指实施常数传播，从而就不必再为它们保留内存。

```c++
class Widget
{
  	public:
    	static const std::size_t MinVals = 28;			//给出MinVals的声明
    	...;
};

//未给出MinVals的定义
std::vector<int> widgetData;
widgetDate.reserve(Widget::Minvals);					//此处用到了MinVals


在这里，尽管Widget::Minvals并无定义，我们还利用了MinVals来指定widgetData的初始容量。编译器绕过了MinVals缺少定义的事实，手法是把值28塞到所有提及MinVals之处。
```



根据上述的预备知识，



```c++
void f(std::size_t val);

以MinVals直接调用f没问题，因为编译器会用MinVals的值来代替它自己：;
f(Widget::MinVals);				//没问题，当28处理

fwd(Widget::MinVals);			//错误！应该无法链接，因为不能取址，没有内存
```



###### 重载的函数名字和模板名字

假设`f`想通过传入一个执行部分操作函数来自定义其行为。假定该函数接受并返回的型别`int`，那么`f`可以声明如下：

```c++
void f(int (*pf)(int));			//pf是process function的简称

值得一提的是，f也可以使用平凡的非指针语法来声明。;

void f(int pf(int));			//声明与上面含义相同f

int processVal(int val);
int processVal(int value, int priority);

f(processVal);		//没问题

fwd(processVal);		//哪个重载版本
```



###### 位域

最后一种完美转发失败情形，是位域被用作函数实参。

```c++
struct IPv4Header{
  	std::uint32_t version:4,
    IHL:4,
    DSCP:6,
    ECN:2,
    totalLength:16;
};

//待调用的函数
void f(std::size_t sz);			

IPv4Header h;

f(h.totalLength);		//没问题

fwd(h.totalLength);		//错误。

这是因为h.totalLength是个非const的位域，而C++规定 非const引用不得绑定到位域。;
解决办法;
//复制位域值，初始化形参
auto length = static_cast<std::uint16_t>(h.totalLength);

fwd(length);		//转发该副本
```



###### 总结

- 完美转发的失败情形，是源于模板型别推导失败，或推导结果是错误的型别。
- 会导致完美转发失败的实参种类有大括号初始化物、以值0或NULL表达的空指针，仅有声明的整型`static const`成员变量、模板或重载的函数名字，以及位域。



