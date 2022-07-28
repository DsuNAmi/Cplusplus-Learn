# 继承与面向对象设计

##  确定你的`public`继承塑造出`is-a`关系

公开继承意味着是一种的关系（is-a）

```c++
class Person {};
class Student : public Person {};

//每个学生是人，但是不是每个人都是学生
```



在公开继承的前提下，有一个论点：

**任何函数如果期望获得一个类型为Person的实参，那么也愿意接受一个Student对象**

对于继承关系，采取编译期拒绝企鹅飞行的设计，而不是只在运行期才侦测它们。



来看一个正方形的例子：

```c++
class Rectangle
{
    virtual void setHeight(int newHeight);
    virtual void setWidth(int newWidth);
    virtual int height() const;
    virtual int width() const;
};

void MakeBigger(Rectangle & r)
{
    int oldHeight = r.height();
    r.setWidth(r.width() + 10);
    assert(oldHeight == r.height());		//始终为真，因为不改变高度/
};

//现在用正方形继承矩形
class Square : public Rectangle {...;}
Square s;

assert(s.width() == s.height());			//为真
makeBigger(s);								//增加宽度

assert(s.width() == s.height());			//按理说应该是正，但是不是

```



这里的问题是，某些可施行于矩形身上的事情却不能施于正方形身上，比如独立修改宽度或高度。

但是公有继承主张：**能够施加在基类身上的每件事情，也可以施行于派生类身上**



==公有继承意味着is-a。适用于基类身上的每一件事一定也适用于派生类对象，因为每一个派生类对象也是一个基类对象。==



## 避免遮掩继承而来的名称

内层作用域的名称会遮掩外围作用域的名称：

```c++
int x;
void someFunc()
{
    double x;
    std::cin >> x;			//local的
}
```



看一个名称查找的过程：

```c++
class Base
{
    private:
		int x;
    public:
    	virtual void mf1() = 0;
    	virtual void mf2();
    	virtual void mf3();
};

class Derived::public Base
{
    virtual void mf1();
    void mf4();
    
};

//实现
void Derived::mf4()
{
    mf2();
}
```



查找顺序：本地作用域->派生类作用域->基类作用域->名称空间作用域->全局作用域。



再来看一个比较复杂的例子：

```c++
class Base
{
  	private:
    	int x;
    public:
    	virtual void mf1() = 0;
    	virtual void mf1(int);
    	virtual void mf2();
    	void mf3();
    	void mf3(double);
};

class Derived::public Base
{
    public:
    	virtual void mf1();
    	void mf3();
    	void mf4();
};



Derived d;
int x;
...;
d.mf1();				//没问题，调用Derived::mf1()
d.mf1(x);				//错误，派生类遮掩了基类
d.mf2();				//调用Base::mf2()
d.mf3();				//调用Derived::mf3()
d.mf3(x);				//错误，

派生类里面的mf3()遮掩了一个基类中名为mf3()但是参数类型不同的函数。;


//可以使用using达到目标
class Derived::public Base
{
    public:
    	using Base::mf1();
    	using Base::mf3();
    	virtual void mf1();
    	void mf3();
    	void mf4();
};//这样就没问题了。
```



有时候你并不想继承基类的所有函数。在公有继承中，这不可能发生，因为违反了公有继承的is-a关系。

假设派生类以私有继承基类，但是派生类唯一想继承的是mf()的无参版本，我们可以考虑使用转交函数，using声明式不再有效，因为在私有继承下，基类的名称在派生类中不可见。

```c++
#include <iostream>

using namespace std;

class Base
{
    public:
        virtual void mf1() {cout << "Im Base";}
        virtual void mf1(int) {cout << "mf2";}
};

class Derived : private Base
{
    public:
        virtual void mf1() {cout << "fdaf"; Base::mf1();}        //转交函数
};


int main()
{
    Derived d;
    int x;
    d.mf1();		//输出fdaf Im Base
    return 0;
}
```



==派生类内的名称会遮掩基类内的名称。在公有继承下从来没有人希望如此==

==为了让被遮掩的名称再见天日，可以使用using或者是转交函数==



## 区分接口继承和实现继承

公有继承概念由两部分组成：**函数接口继承和函数实现继承。**

有时候你希望派生类只继承成员函数的接口，有时候又希望同时继承接口和实现，但是又希望能够复写其实现，但有时候又不希望复写。

来看一个例子：

```c++
class Shape
{
  	public:
    	virtual void draw() const = 0;				//纯虚
    	virtual void error(const std::string& msg);	//非纯虚函数
    	int objectID() const;						//非虚函数
};

class Rectangle : public Shape {};
class Ellopse : public Shape {};
```

该例子有以下特点：

- 成员函数的接口总是会被继承。

- 声明一个纯虚函数的目的是为了让派生类只继承函数接口。

  纯虚函数的两个最主要的特性：1 它们必须被任何继承了它们的具象类重新声明。2 在抽象类中不定义纯虚函数。（但是我们仍然可以定义纯虚函数）

  ```c++
  class Time
  {
      public:
          double time;
          Time() : time(0) {}
          virtual void GoTime() = 0;
          virtual void Notime() {cout << "NoTime Now";}
  };
  
  void Time::GoTime()
  {
      cout << "Im pure virtual";
  }
  
  class subTime : public Time
  {
      public:
          void GoTime() {cout << "Time goes on...";}
          double GetTime() {return this->time;}
          void Notime() {cout << "Now No Time";}
  };
  
  int main()
  {
      Time * t = new subTime();
      t->Time::GoTime();		//调用定义的纯虚函数
      t->Notime();
      subTime st;
      cout << st.GetTime() << endl;
      return 0;
  }
  ```

  

- 声明非纯虚函数的目的，是为了让派生类继承该函数的接口和缺省实现。

  对于

  ```c++
  virtual void error(const std::string & msg);
  ```

  该接口表示，每个类都必须支持一个报错函数。但是每个类可以自由处理错误函数。如果不针对他做出错误反馈，那么就退回到缺省的行为。（非纯虚函数必须要定义）

  但是这样可能会造成一些危险

  ```c++
  class Airport {};
  class Airplane
  {
      public:
      	virtual void fly(const Airport & destination);
  };
  
  void Airplane::fly(const Airport & destination)
  {
      //default code
  }
  
  class ModelA : public Airplane {};
  class ModelB : public Airplane {};
  ```

  为了表示所有飞机都能飞，且不同飞机的飞实现不同。就用非纯虚函数

  但是现在如果加入了第三驾飞机且忘记编写fly函数。

  ```c++
  class ModelC : public Airplane {};
  Airport PDX();
  Airplane * pa = new ModelC();
  pa->fly(PDX);//这个时候会调用ModelA或者ModelB的飞行函数
  ```

  解决的方法如下：

  ```c++
  class Airplane
  {
      public:
      	virtual void fly(const Airport & destination) = 0;
      protected:
      	void defaultFly(const Airport & destination);
  };
  
  void defaultFly(cont Airport & destination)
  {
      //default code
  }
  
  class ModelA : public Airplane
  {
      public:
      	virtual void fly(const Airport & destination)
          {
              defaultFly(destination);
              //or other code
          }
  };
  
  因为定义了纯虚函数，这个时候ModelC就必须实现fly函数。
  ```

- 声明非虚函数就是的目的是为了令派生类继承函数的接口及一份强制性实现。

  如果一个函数是非虚函数，那么就并不打算在派生类中有不同的行为。其不变性凌驾于特异性。



==接口继承和实现继承不同。在public继承之下，派生类总是继承基类的接口==

==纯虚函数只具体指定接口继承==

==非纯虚函数具体指定接口继承以及缺省实现继承==

==非虚函数具体指定接口继承以及强制性实现继承==



## 考虑virtual以外的其他选择

###### 借由non-virtual interface 手法实现template method模式

主要思想是虚函数总是私有函数。

```c++
class GameCharacter
{
  	public:
    	int healthValue() const
        {
            ...;							//做一些事前工作
            int retVal = doHealthValue();	//做真正的工作
            ...;							//做一些事后工作
            return retVal;
        }
    private:
    	virtual int doHealthValue() const
        {
            ...;//具体实现
        }
};
```

客户通过非虚公有函数调用私有虚函数。，称为non-virtual interface（NVI）手法。这个虚函数称为外覆器。（wrapper）

###### 借由Function Pointer 实现Strategy模式

```c++
class GameCharacter;				//前置声明
int defaultHealthCalc(const GameCharacter & gc);

class GameCharacter
{
  	public:
    	typedef int (*HealthCalFunc)(const GameCharacter & gc);
    	explicit GameCharacter(HealthCalcFunc hcf = defaultHealthCalc)
            : healthFunc(hcf) {}
    	int healthValue() const {return healthFunc(*this);}
    private:
    	HealthCalcFunc healthFunc;
};
```

对于上述设计模式：

- 同一人物类型之不同实体可以有不同的健康计算函数

  ```c++
  class EvilBadGuy : public GameCharacter
  {
      public:
      	explicit EvilBadGuy(HealthCalcFunc hcf = defaultHealthCalc)
              : GameCharacter(hcf) {}
  };
  
  int LoseHealthQuickly(const GameCharacter&);
  int LoseHealthSlowly(const GameCharacter&);
  
  EvilBadGuy ebg1(LoseHealthQuickly);
  EvilBadGuy ebg2(LoseHealthSlowly); 
  
  可以看到同一个类型的不同对象使用了不同的函数。
  ```

- 某已知人物的健康指数函数可在运行时期更变。

- 不能访问类的非公有部分。

###### 借由tr1::function 完成Strategy模式

如果我们不再使用函数指针，而是改用一个类型`tr1::function`的对象，这样的对象可以持有任何可调用物，比如函数指针，函数对象或成员函数指针。

```c++
class GameCharacter;
int defaultHealthCalc(const GameCharacter& gc);
class GameCharacter
{
    public:
    	typedef std::tr1::function<int (const GameCharacter&)> HealthCalcFunc;
    	//接受一个const GameCharacter并返回一个int
    	//可调用的参数被隐式转换成GameCharacter以及隐式转换成int
    	explicit GameCharacter(HealthCalcFunc hcf = defaultHealthCalc)
            : healthFunc(hcf) {}
    	int healthValue() const {return healthFunc(*this);}
    private:
    	HealthCalcFunc healthFunc;
};

//可以传递的参数包括如下：
short calcHealth(const GameCharacter&);

struct HealthCalculator
{
    int operator()(const GameCharacter&) const {}
};

class GameLevel
{
  	public:
    	float health(const GameCharacter&) const;
};
```

###### 古典strategy模式

将继承体系的virtual函数替换为另一个继承体系内的virtual函数。

```c++
class Character;
class HealthCalcFunc
{
  	  public:
    	...;
    	virtual int calc(const GameCharacter& gc) const {}
    	...;
    
};

HealthCalcFunc defaultHealthCalc;

class GameCharacter
{
  	public:
    	explicit GameCharacter(HeathCalcFunc * phcf = &defaultHealthCalc)
            ： pHealthCalc(phcf){}
    	int healthValue() const {return pHealthCalc->calc(*this);}
    private:
    	HealthCalcFunc * pHealthCalc;
};
```

==virtual函数的替代方案包括NVI手法及Strategy设计模式的多种形式。==

==将机能从成员函数移到class外部函数，带来一个缺点是，非成员函数无法访问类的非公有成员==

==tr1:function对象的行为就像一般函数指针。这样的对象可接纳 与给定之目标签名式兼容的所有可调用物。==



## 绝不重新定义继承而来的non-virtual函数。

因为非虚函数是静态绑定的，而虚函数时动态绑定的。对于非虚函数重载，即使使用指向派生类对象的基类指针第调用，也是运行基类中的成员函数，而不是派生类。



==绝对不要重新定义继承而来的非虚函数指针==



## 绝不重新定义继承而来的缺省值参数

因为非虚函数在上一个条款中明确说明不让重新定义，那么这句话也可以理解成不要继承带有一个缺省参数的虚函数。

因为**virtual函数系统动态绑定，而缺省参数值确实静态绑定。**

来看一个例子：

```c++
class Shape
{
  	public:
    	enum ShapeColor {Red, Green, Blue};
    	//所有形状都必须提供绘制自己的函数
    	virtual void draw(shapeColor color = Red) const = 0;
};


class Rectangle
{
  	public:
    	//修改了缺省参数
    	virtual void draw(shapeColor color = Green) const;
};

class Circle : public Shape
{
    virtual void draw(shapeColor color) const;
    //如果这么写，用户以对象调用该函数，一定要写参数。因为静态绑定下并不从其Base继承缺省值
    //如果是指针引用，那么可以，动态绑定会从基类继承缺省值。
};

//考虑下面指针
Shape * ps;
Shape * pc = new Circle;
Shape * pr = new Rectangle;

//三者的静态类型都一样，但是动态绑定的内容是目前所指向的对。

pr->draw()。;
//这里会调用Red，而不是Green。因为继承的是基类型。
```

按照条款的说明。怎么办。可以采用NVI手法。

```c++
class Shape
{
  	public:
    	enum ShapeColor{Red, Green, Blue};
    	void draw(ShapeColor color = Red) const
        {
            doDraw(color);
        }
    private:
    	virtual void doDraw(shapeColor color) const = 0;
    
};

class Rectangle : public Shape
{
  	public:
	...;
    private:
    	virtual void doDraw() const; //这样默认参数就永远是红色
};
```



==绝对不要定义一个继承而来的缺省参数值，因为缺省参数值都是静态绑定，而virtual函数是动态绑定==



## 通过复合塑造出has-a或根据某物实现出

复合是类型之间的一种关系，当某种类型的对象内含它中类型的对象，便是这种关系。

简单来说就是类中含有类。

复合意味着has-a或is-implemented-in-terms-of（根据某物实现出）。

当我们需要实现一个自己的`set`时，我们可能会直接继承`Set`，但是因为我们可能对空间要求更高。所以我们选择继承`list`，但是继承`list`并不满足is-a的关系，因为对于`list`来说可以存储重复元素而`set`不行。这时候我们可以采用复合的方式来实现：

```c++
template<typename T>
class Set
{
  	public:
    	bool member(const T& item) const;
    	void insert(const T& item);
    	void remove(const T& item);
    	std::size_t size(); const;
    private:
    	std::list<T> rep;
};

template<typename T>
bool Set<T>::member(const T& item)
{
    return std::find(rep.begin(),rep.end(), item) != rep.end();
}

template<typename T>
void Set<T>::insert(const T& item)
{
    if(!member(item)) rep.push_back(item);
}

template<typename T>
void Set<T>::remove(const T& item)
{
    typename std::list<T>::iterator it = 
        std::find(rep.begin(),rep.end(),item);
    if(it != rep.end()) rep.erase(it);
}

template<typename T>
std::size_t Set<T>::size() const
{
    return rep.size();
}
```



==复合的意义和公有继承完全不同==

==在应用域，复合意味has-a。而在实现域，复合以为者根据某物实现出==



## 明智而审慎地使用私有继承

私有继承：**编译器不会自动将一个派生类对象转换为一个基类对象。从基类中继承而来的所有成员，在派生类中都会变成私有属性，不管原来是什么。**

私有继承意味着  根据某物实现出。所以尽可能使用复合，必要时才使用私有继承。

==私有继承意味着 根据某物实现出。它通常比复合地级别低。但是当派生类需要访问保护基类的成员，或需要重新定义继承而来的虚函数时，这么设计是合理的。==

==和复合不同，私有继承可以造成empty base最优化。这对致力于 对象尺寸最小化 的程序库开发者而言，可能很重要==



## 明智而审慎地使用多重继承MI（multiple inheritance）

多重继承有可能从一个以上的基类继承相同名称。那会造成较多的歧义。

例如：

```c++
class BorrowableItem
{
  	public:
    	void checkOut();				//离开时检查
};

class ElectronicGadget
{
    private:
    	bool checkOut() const;
};

class MP3Player : public BorrowableItem, public ElectronicGadget {};

MP3Player mp;
mp.checkOut();		//调用哪个函数？
```



即使其中一个时私有成员其语义也是模棱两可的，为了解决这个歧义可以使用`mp.BorrowableItem::checkOut()`，调用另一个则会报错。



我们来看看菱形继承问题：

```c++
class File {};
class InputFile : public File {};
class OutputFile : public File {};
class IOFile : public InputFile, public OutputFile {};

//为了防止IOFile，出现两次File类中的成员，必须使用virtual 继承
class File {};
class InputFile : virtual public File {};
class OutputFile : virtual public File {};
class IOFile : public InputFile, public OutputFile {};
```



但是虚拟继承之后会扩大继承的类的大小。



- 非必要不适用虚基类。
- 避免在虚基类中放置数据。



下面来看一个例子，如何从一个抽象类中实体化

```c++
class IPerson
{
    public:
    	virtual ~IPerson();
    	virtual std::string name() const = 0;
    	virtual std::string birthday() const = 0;
};

std::tr1::shared_ptr<IPerson> makePerson(DatabaseID personIdentifier);
//根据独一无二的数据库ID创建一个IPerson对象。

//获得ID
DatebaseID askUserForDatabaseID();

DatabaseID id(askUserForDatabaseID());

std::tr1::shared_ptr<IPerson> pp(makePerson(id));
//创建一个对象支持Iperson接口

那么这时候肯定需要一些派生来的非抽象类来创建对象;
我们假设这个类为CPerson，但是我们先有个数据库相关的类，提供CPerson所需要的实质性东西;

class PersonInfo			//被设计用来打印数据库字段
{
  	public:
    	explicit PersonInfo(DatabaseID pid);
    	virtual ~PersonInfo();
    	virtual const char* theName() const;
    	virtual const char* theBirthDate() const;
    	...;
    private: //后面继承的时候换成Public
    	virtual const char* valueDelimOpen() const;
    	virtual const char* valueDelimClose() const;
    	...;
};

比如Ring-tailed Lemur变为[Ring-tailed Lemur];

const char* PersonInfo::valueDelimOpen() const
{
    return "[";
}

const char* PersonInfo::valueDelimClose() const
{
    return "]";
}

const char* PersonInfo::theName() const
{
    //保留缓冲区给返回值使用
    //由于缓冲区是static
    //因为会被自动初始化为0
    static char value[Max_Formatted_Field_Value_Length];
    
    std::strcpy(value,valueDelimOpen());
    //添加value的内的字符串到name中
    std::strcpy(value,valueDelimCloase());
    
    return value;
}

//为了结合PersonInfo和IPerson的功能。
//将public继承自某接口，private继承自某实现
class CPerson : public IPerson, private PersonInfo
{
  	public:
    	explicit CPerson(DatabaseID pid) : PersonInfo(pid) {}
    	virtual std::string name() const {return PersonInfo::theName();}
    	virtual std::string birthDay() const {return PersonInfo::theBirthday();}
    private:
    	const char* valueDelimOpen() const {return "";}
    	const char* valueDelimClose() const {return "";}
};
```



==多重继承比单一继承复杂。它可能导致新的歧义性，以及对virtual继承的需要。==

==virtual继承会增加大小、速度、初始化复杂度成本等等。如果virtual base class不带任何数据，将是最具实用价值的情况==

==多重继承的确有正当用途。其中一个情节涉及公有继承某个接口类，私有继承某个协助实现类==

