# 实现

## 尽可能延后变量定义式出现的时间

只要你定义了一个变量而其类型带有一个构造函数和析构函数，那么当程序的控制流到达这个变量定义式的时候，你便得承受构造函数成本；当这个变量离开作用域时，你得承受析构成本。

我们来看一个例子，计算同行密码的加密版本而返回。

```c++
//这个函数过早定义变量
std::string encryptPassword(const std::string & password)
{
    using namespace std;
    string encrypted;
    if(password.length() < MinimumPasswordLength)
    {
        throw logic_error("Password is too short");		//并没有使用encrypted
    }
    ...;			//必要动作，能将一个加密后的密码植入encrypted内
    
    return encrypted;
}
```



我们可以尝试延后`encrypted`的定义，但是仅仅只是把第5行代码移动，这里没有给初始值，将会调用默认构造函数。我们应该在定义`encrypted`时给其赋予初值。



所以，你不只应该延后变量的定义，直到非得使用变量的一刻为止，甚至应该尝试延后这份定义到能够给它初值为止。



那么问题来了，如果遇到循环怎么办？**变量只需要在循环内**，我们参考下面两端代码：

```c++
//方法1，定义于循环外
Widget w;
for(int i = 0; i < n; ++i)
{
    w = someValue;			//一个构造函数+一个析构+n个赋值构造函数
    ...;
}

//方法2，定义与循环内
for(int i = 0; i < n; ++i)
{
    Widget w = someValue;	//n个构造函数+n个析构函数
    ...;
}
```



如果类的一个赋值成本低于一组构造函数和析构函数，做法1大体而言效率较高，尤其是当n比较大的时候。否则2效率更高。

此外方法1造成名称w的作用域比作用B更大，有时候会对程序的可理解性和易维护性造成冲突。

**所以应该尽可能使用方法2**。



==尽可能延后变量定义式的出现。这样可增加程序的清晰度并改善程序效率==



## 尽量少做转型动作

C++规则的设计目标之一是保证**类型错误**绝不可能发生。

`static_cast`不能将`const`转为`non-const`，这个只能用`const_cast`。



许多程序员相信，转型其实什么都没做，只是告诉编译器把某种类型视为另外一种类型，这是错误的概念。

任何一个类型转换往往真的是令编译器译出运行期间执行的代码。

比如：

```c++
class Base {...;};
class Derived : public Base {...;};
Derived d;
Base * b = &d;
```



上述例子建立一个基类指针指向派生类对象，但是有时候上述的两个指针并不相同。这时候会有一个`offset`在运行期间被施予派生类指针身上用来取得正确的基类指针。



也就是说，单一对象可能会有一个以上的地址，上述例子中，就有可能有`Base *`指向 它时的地址和`Derived *`指向它时的地址。



同样，我们再来看一个例子：

```c++
class Window
{
  	public:
		virtual void onResize() {...;}
    
};

class SpecialWindow : public Window
{
  	public:
    	virtual void onResize()
        {
            static_cast<Window>(*this).onResize();//转型成window的对象调用，错误写法
            Windows::onResize();			//正确写法
            
            //这里是SpecialWindow代码
        }
};
```



你以为这段会调用`Windows::onResize()`，并不是，而是稍早转型动作所建立的一个`*this`对象的基类的暂时副本身上的`onResize()`。如果说`OnResize()`中有改动成员变量的内容，那么使用错误的代码就无法实现改动，改动的其实是副本。

如果你发现你自己打算转型，那其实是一个警告，你可能正将局面发展至错误的方向上。



之所以需要`dynamic_cast`，通常是因为你想在一个你认定为派生类对象身上执行派生类操作函数，但是你手上只有一个指向基类的指针，有两个一般性做法可以解决这个问题：

第一，使用容器并在其中存储直接指向派生类对象的指针。

```c++
//错误示范
class Window {...;};
class SpecialWindow : public Window {
    public:
    	void blink();
    
};

typedef std::vector<std::tr1::shared_ptr<Window>> VPW;
VPW winPtrs;
...;
for(VPW::iterator iter = winPtrs.begin();
    iter != winPtrs.end();
    ++iter;)
{
    if(SpecialWindow * psw = dynamic_cast<SpecialWindow*>(iter->get()))
    {
        psw->blink();			//这里有时候转不了，基类不能转派生类，使用dynamic_cast会检测
    }
}

//正确的写法
typedef std::vector<std::tr1::shared_ptr<SpecialWindow>> VPSW;
VPSW winPtrs;
for(VPSW::iterator iter = winPtrs.begin();
    iter != winPtrs.end();
    ++iter;)
{
    (*iter)->blink();
}

//但是缺点就是只能指向SpecialWindow
```



另一种方法就是在`Window`内部也写一个`blink()`函数，即使它什么也没做。



==如果可以，尽量避免转型，特别是在注重效率的代码中避免dynamic_cast，如果有个设计需要转型动作，试着往无需转型的替代设计==

==如果转型是必要的，试着将它隐藏于背后某个函数。客户随后可以调用该函数，而不需要将转型放进他们自己的代码==

==宁可使用C++新式转型==



## 避免返回handles指向对象内部成分

假设你的程序涉及到矩形。为了让一个矩形的对象尽可能小，可能会把矩形的点放在一个结构体中存储。

```c++
class Point
{
  	public:
		Point(int x, int y);
    	...;
    	void setX(int newVal);
    	void setY(int newVal);
    	...;
};

struct RectData
{
  	Point ulhc;			//矩形左上角的点
    Point lrhc;			//矩形右下角的点
};

class Rectangle
{
  	public:
    	...;
    	Point & upperLeft() const {return pData->ulhc;}
    	Point & lowerRight() const {return pData->lrhc;}
    	...;
    private:
    	std::tr1::shared_ptr<RectData> pData;
};//可以通过编译，但是实际上是错误的。
一方面声明const,但是另一方面又返回私有成员变量的引用，这样就能给让别人修改数据。;

Point coord1(0,0);
Point coord2(100,100);
const Rectangle rec(coord1,coord2);

rec.upperLeft().setX(50);		//这里就修改了值，因为返回了私有对象的引用。


```



如果它们返回的是指针或迭代器，相同的情况还是会发生，原因也相同。引用、指针和迭代器统统都是所谓的`handles`，（句柄，用来取得某个对象），而返回一个**代表对象内部数据**的句柄，随之而来的是降低封装性的封风险。同时，也可能导致**虽然调用const成员函数却造成对象状态被改变。**



如果我们尝试：

```c++
class Rectangle
{
    public:
		...;
    	const Point& upperLeft() const {return pData->ulhc;}
    	const Point& lowerRight() const {return pData->lrhc;}
}
```



但是上述代码可能会导致**空悬的句柄**，这种句柄所指向的东西的所属对象不复存在。例如：

```c++
class GUIObject {...;};
const Rectangle boundingBox(const GUIObject& obj);

//现在，客户可能有这么一个函数
GUIObject * pgo;
...;
const Point* pUpperLeft = &(boundingBox(*pgo).upperLeft());
当boundingBox返回的对象析构后，pUpperLeft指向的对象将不复存在。
```



==避免返回引用、指针、迭代器指向对象内部。遵守这个条件条款可以增加封装性，帮助const成员函数的行为像个const，并将发生悬空句柄的可能性降至最低。==



## 为“异常安全”努力是值得的

假设有个类用来表现夹带背景图案的GUI菜单。

```c++
class PrettyMenu
{
    public:
    	...;
    	void changeBackground(std::istream & imgSrc);		//改变背景图像
    	...;
   	private:
    	Mutex mutex;			//互斥器
    	Image* bgImage;			//目前的背景图像
    	int imageChanges;		//背景图像的改变次数
};

void PrettyMenu::changeBackground(std::istream & imgSrc)
{
    lock(&mutex);		//取得互斥器
    delete bgImage;		//摆脱旧的背景图像
    ++imageChanges;		//修改图像更变次数
    bgImage = new Image(imgSrc);
    unlock(&mutex);
}

```



从异常安全性的角度，这个函数很糟，因为没有满足其两个条件：

- **不泄露任何资源**。因为一旦`new Image(imgSrc)`导致异常，那么`unlock`就不会执行，那么互斥器就被卡住了。
- **不允许数据败坏**。同样创建对象失败，那么`bgImage`指向一个不存在的对象，但是`imageChages`还是更行了。



异常安全函数提供以下三个保证之一：

- **基本承诺**：如果异常被抛出，程序内的任何事物仍然保持在有效状态下。没有任何对象或数据结构会因此而败坏，所有对象都处于一种内部前后一致的状态。然而程序的现实状态恐怕不可预料。
- **强烈保证**：如果异常被抛出，程序状态不变。调用这样的函数需有这样的认知：如果函数成功，就是完全成功，如果函数失败，程序会回复到“调用函数之前的状态”。
- **不抛掷保证**：承诺绝不抛出异常，因为它们总是能够完成它们原先承诺的功能。比如内置类型就有这个保证。



我们可以尝试修改上述代码，使他满足异常安全函数提供的保证：

```c++
class PrettyMenu
{
  	...;
    std::tr1::shared_ptr<Image> bgImage;
    ...;
};


void PrettyMenu::changeBackground(std::istream & imgSrc)
{
    Lock ml(&mutex);			//之前写的互斥锁的对象
    bgImage.reset(new Image(imgSrc)); 		//以new Image()执行的结果设定指针
    ++imageChanges;							//不要为了表示某件事发生而改变对象状态，除非这件事真的发生了。
}
```



有一种策略叫做`copy and swap`，为你打算修改的对象做出一件副本，然后在副本身上做一切必要修改。若有任何修改动作抛出异常，原对象仍未改变状态。待一切都正常完成后，再将修改过的那个副本和原对象在一个不抛出异常的操作中置换。



实现上通常称为：pimpl idiom

```c++
struct PMImpl	//因为在PrettyMenu，已经是私有的，所有struct足矣。
{
  	std::tr1::shared_ptr<Image> bgImage;
    int imageChanges;
};

class PrettyMenu
{
  	...;
    private:
    	Mutex mutex;
    	std::tr1::shared_ptr<PMImpl> pImpl;
};

void PrettyMenu::changeBackground(std::istream & imgSrc)
{
    using std::swap;
    Lock ml(&mutex);
    std::tr1::shared_ptr<PMImpl> pNew(new PMIpml(*pImpl));
    //修改副本
    pNew->bgImage.reset(new Image(imgSrc));
    ++pNew->imageChanges;
    
    swap(pImpl,pNew);
}
```



一个软件系统要么就具备异常安全性，要不就全然否定，没有所谓的局部异常安全系统。如果系统内有一个函数不具备异常安全性，整个系统就不具备异常安全性，因为调用那个函数可能导致资源泄露或数据结构败坏。



==异常安全函数即使发生异常也不会泄露资源或允许任何数据结构败坏。这样的函数分为三种可能的保证：基本型、强烈型、不抛异常型==

==强烈保证往往能够以copy-and-swap实现出来，但强烈保证并非对所有函数都可实现或具备现实意义。==

==函数提供的异常安全保证通常最高只等于其所调用之各个函数的异常安全保证中的最弱者。==



## 透彻了解inlining的里里外外

`inline`函数的背后的整体观念是，将**对此函数的调用**都以函数本体替换之。这样做可能增加你的目标码的大小。在一台内存有限的机器上，过度热衷`inline`会造成程序体积太大，即使拥有虚内存，`inline`造成的代码膨胀也会导致额外的换页行文，降低指令高速缓存装置的击中率。



注意，`inline`只是对编译器的一个申请，不是强制命令。这项申请可以隐喻提出，也可以明确提出。

```c++
//隐喻，将函数定义在class内部。
class Person
{
  	public:
		...;
    	int age() const {return theAge;}		//隐喻
    	...;
    private:
    	int theAge;
};

//明确提出，加上inline
template <typename T>
inline const T& std::max(const T& a, const T& b)
{return a < b ? b : a;}
```



`inline`函数通常一定被置于头文件内，因为大多数建置环境在编译过程中进行`inlining`，而为了将一个函数调用替换为被调用函数的本体，编译器必须直到那个函数长什么样子。`inlining`在C++大多数程序中是编译器行文。



因为`inline`只是一个申请，所以编译器将拒绝太过复杂（例如带有循环和递归）的函数`inlining`，而对所有虚函数调用也都会时`inlining`落空，因为虚函数意味着直到运行期才知道调用哪个函数，而`inline`意味着执行前，先将调用动作替换为被调用函数的本体。



也就是说：**一个表面上看似inline的函数是否真是inline，取决于你的建置环境，主要取决于编译器。如果它们无法将你要求的函数inlining，那么它们会提出警告。**



```c++
//满足inline条件的非inling
inline void f() {...;}

void (*pf)() = f;

f();					//inlined
pf();					//不一定inlined
```



==将大多数inlining限制在小型、被频繁调用的函数身上。这可使日后的调式过程和二进制升级更容易，也可以使潜在的代码膨胀问题最小化，使程序的速度提升机会最大化。==

==不要只因为模板函数出现在头文件，就将它们声明为inline==



## 将文件间的编译依存关系降至最低

C++并没有把**将接口从实现中分离**这事做得很好。类的定义式不只详细叙述了类接口，还包括十足的实现细目。

```c++
class Person
{
    public:
    	Person(const std::string & name, const Date& birthday, const Address& addr);
    	std::string name() const;
    	std::string birthDate() const;
    	std::string address() const;
    private:
    	//实现细目
    	std::string theName;
    	Date theBirhDate;
    	Address theAddress;
    	//只提供接口
    	class string;
    	class Date;
    	class Address;
    	//这是不行的，因为编译器不知道每个类的大小，不能分配空间。
};

这里的类无法通过编译，因为缺少;
#include <string>
#include "date.h"
#inlcude "address.h"
```



那么这样的话，这些文件之间就存在一种编译依存关系。如果其中的一个头文件内容被改变，那么另一个文件中的内容也都要改变。

我们来看一个接口和实现分离的例子，也就是`pimpl idiom`是，pointer to implementation的缩写。，这类指针往往就是`pImpl`。



```c++
#include <string>
#include <memory>

class PersonImpl;				//Person类的前置声明
class Date;						//Pseron接口用到的前置声明
class Address;

class Person
{
  	public:
    	Person(const std::string & name, const Date& birthday, const Address& addr);
    	std::string name() const;
    	std::string birthDate() const;
    	std::string address() const;
    private:
    	std::tr1::shared_ptr<PersonImpl> pImpl; 		//指针，指向实物
};
```



这样的设计之下，`Person`的客户就完全与Date，Address以及Person的实现细目分离了。

- 如果使用对象引用或者是对象指针可以完成任务，就不要使用对象。
- 如果能够，尽量以类声明式替换定义式。
- 为声明式和定义式提供不同的头文件。



一个针对Person写的接口类看起来像这样：

```c++
#include <iostream>
#include <string>
#include <memory>


using namespace std;

class Date;
class Address;


class Person
{
    public:
        virtual ~Person();
        virtual string name() const = 0;
        virtual string birthDate() const = 0;
        virtual string address() const = 0;
        static shared_ptr<Person> create(const string & name, const Date & birthday, const Address & addr);
};


class RealPerson : public Person
{
    public:
        RealPerson(const string &name, const Date& birthday, const Address & addr)
        : theName(name),theBirth(birthday),theAddr(addr) {}
        virtual ~RealPerson();
        string name() const;
        string birthDate() const;
        string address() const;
    private:
        string theName;
        Date theBirth;
        Address theAddr; 
};  

shared_ptr<Person> Person::create(const string & name, const Date & birthday, const Address & addr)
{
    return shared_ptr<Person>(new RealPerson(name,birthday,addr));
}



int main()
{
    string name;
    Date dateofBirth;
    Address address;


    //创建一个接口对象
    shared_ptr<Person> pp(Person::create(name,dateofBirth,address));
    cout << pp->name() << pp->birthDate() << pp->address();
    return 0;
}


```



但其实如果不定义`Date`和`Address`，这个程序是不能被编译的。但是我们可以只引入声明的头文件，让声明中再去定义吧。



==支持编译依存性最小化的一般构想是：相依于声明式，不要相依与定义式。基于此构想的两个手段是句柄类和接口类。==

==程序库头文件应该以完全且仅有声明式的形式存在。这种做法不论是否==

