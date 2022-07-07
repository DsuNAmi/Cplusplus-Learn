# 纯虚函数和抽象类

## 简介

c++中的纯虚函数（或抽象函数）是我们没有实现的虚函数！我们只需要声明它！通过声明中赋值0来声明纯虚函数。

```c++
//抽象类
class A{
    public:
    	virtual void show() = 0; //纯虚函数
    	/*other members*/
}
```

- 纯虚函数：没有函数体的虚函数
- 抽象类：包含纯虚函数的类。

抽象类只能作为基类派生新类使用，不能创建抽象类的对象，抽象类的指针和引用指向由抽象类派生出来的类的对象。

> 参考代码：pureVirtual.cpp

## 实现抽象类

抽象类中：在成员函数内可以调用纯虚函数，在构造函数或者是析构函数内部不能使用纯虚函数。

如果一个类从抽象类派生而来，它必须实现了基类中的所有纯虚函数，才能成为非抽象类。

```c++
//A 为抽象类
class A
{
    public:
    	virtual void f() = 0;
    	void g() {this->f();}
    	A() {}
};

class B : public A
{
    public:
    	void f(){cout << "B:f()" << endl;}
};
```

## 重要点

- 纯虚函数使一个类变成抽象类。

```c++
//抽象类至少包含一个纯虚函数
class Base
{
    public:
    	virtual void show() = 0; //纯虚函数
    	int getX() {return x;}
    private:
    	int  x;
};
```

- 抽象类类型的指针和引用

```c++
class Derived : public Base
{
    public:
        void show() {cout << "In Derived \n";} //实现了抽象类的纯虚函数
        Derived() {} //构造函数

};


int main()
{
    // Base b; //error 
    Base * bp = new Derived(); //
    bp->show();
    return 0;
}
```

- 如果我们不在派生类中覆盖纯虚函数，那么派生类也会变成抽象类。

```c++
class Derived: public Base
{
    public:
    	//void show() {}
}
```

- 抽象类可以有构造函数

```c++
//抽象类
class Base
{
    protected:
    	int x;
    public:
    	virtual void fun() = 0;
    	Base(int i) {x = i;}
    
};

//派生类
class Derived : public Base
{
    int y;
    public:
    	Derived(int i, int j) : Base(i) {y = j;}
    	void fun()  {cout << "x = " << x << ", y = " << y;}
};
```

- 构造函数不能是虚函数，而析构函数可以是虚析构函数

```c++
//抽象类
class Base
{
    public:	
    	Base() {cout << "Constructor: Base" >> endl;}
    	virtual ~Base() {cout << "Destructor : Base" << endl;}
    	virtual void func() = 0;
};

class Derived : public Base
{
    public:
    	Derived() {cout << "Constructor: Derived" << endl;}
    	~Derived() {cout << "Destructor: Derived" << endl;}
    
    	void func() {cout << "In Derived.func()." << endl;}
};
```

> 当基类指针指向派生类对象并删除对象时，我们可能希望调用适当的析构函数。如果析构函数不是虚拟的，则只能调用基类的析构函数。

## 完整示例

> 参考代码：derivedFull.cpp

