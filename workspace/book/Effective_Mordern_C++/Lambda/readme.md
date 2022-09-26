# Lambda表达式

一些基本术语：

- lambda表达式，是表达式的一种

  ```c++
  std::find_if(container.begin(),container.end(),
               [](int val) {return 0 < val && val < 10;})
  ```

- 闭包是lambda式创建的运行期对象，根据不同的捕获模式，闭包会持有数据的副本或引用。

- 闭包类就是实例化闭包的类。每个lambda式都会触发编译器生成一个独一无二的闭包类。而闭包中的语句会变成它的闭包类成员函数的可执行指令。

```c++
int x; 			//一个局部变量


auto c1 = [x](int y) {return x * y > 55;} //c1是lambda产生的闭包副本


auto c2 = c1;								//c2是c1的副本
auto c3 = c2;								//c3是c2的副本
```



# 条款31，避免默认捕获模式

默认捕获模式：

- 按引用。
- 按值。

按引用的默认捕获模式可能导致空悬引用，按值的默认捕获模式会忽悠你，好像可以对空悬引用免疫，还让你认为你的闭包是独立的。



按引用捕获会导致闭包包含指涉到局部变量的引用，或指涉到定义lambda式的作用域内的形参的引用。一旦由lambda式所创建的闭包越过了该局部变量或形参的声明周期，那么闭包内的引用就会空悬。



来看一个例子：

```c++
using FilterConatainer = std::vector<std::function<bool(int)>>;

FilterConatainer filters;           //元素为删选器的容器

filters.emplace_back(
    [](int value) {return value % 5 == 0;}
);

void addDivisorFilter(FilterConatainer & filters)
{
    //我们可能需要在运算期计算出除数，而不是把硬编码的5写入lambda表达式
    auto calc1 = computeSomeValue1();
    auto calc2 = computeSomeValue2();
    auto divisor = computeDivisor();

    filters.emplace_back(
        [&](int value){return value % divisor == 0;} //危险，对divisor的指针可能空悬
    );
}

改代码随时会出错。lambda式是指涉到局部变量divisor的引用，但是该变量在addDivisorFilter返回时即不再存在。换言之，该变量的销毁就是紧接着filters.emplace_back返回的那一时刻。使用这个筛选器，从它刚被创建的那一刻起，就会产生未定义行为。;

如果采用显式捕获;
filters.emplace_back(
        [&divisor](int value){return value % divisor == 0;}
    );

这样的方式提醒了人们去看到lambda表达式依赖于divisor的声明周期;
```



从长远观点来看，显式地列出lambda表达式所依赖的局部变量或形参是更好的软件工程实践。



对于上述的例子，我们可以采取按值传递的方法来解决该问题：

```c++
filter.emplace_back(
[=](int value){return value % divisor == 0;}
    )
```



但是按值传递也存在一个问题，在lambda式创建的闭包中持有的是这个指针的副本，但你并无办法阻止lambda式之外的代码去针对该指针实施`delete`操作所导致的指针副本空悬。

```c++
class Widget
{
  	public:
    	...;
    	void addFilter() const;
    private:
    	int divisor;
};

void Widget::addFilter() const
{
    filters.emplace_back(
    [=](int value){return value % divisor == 0;}
    )
}

捕获只能针对于在创建lambda表达式的作用域内可见的非静态局部变量，所以其实他根本没有被捕获。;
如果代码写成如下两种形式都不能通过编译，因为divisor并不是局部变量，也不是形参;
filters.emplace_back([](int value){return value % divisor == 0;});
filters.emplace_back([divisor](int value){return value % divisor == 0;});


因为实际上，lambda表达式捕获的其实是this指针:;
void Widget::AddFilter() const
{
    auto currentObjectPtr = this;
    filters.emplace_back(
    [currentObjectPtr](int value)
        {return value % currentObjectPtr->divisor == 0;}
    );
}

所以解决上述办法的一个方式是:使用广义lambda捕获;
void Widget::addFilter() const
{
    filters.emplace_back(
    	[divisor = divisor](int value)
        {return value % divisor == 0;}			//使用的复制进来的副本
    )
}
```



按值捕获的另一个缺点就是让人们以为闭包式自洽的，我们来看一个列子：

```c++
void addDivisorFilter()
{
    static auto calc1 = computeSomeValue1();
    static auto calc2 = computeSomeValue2();
    
    static auto divisor = computeDivisor(calc1, clac2);
    
    filters.emplace_back(
    [=](int value)
        {return value % divisor == 0;}		//根本没有捕获副本，而是static变量
    );
    
    ++divisor;				//这里其实修改了值
}
```



###### 总结

- 按引用的默认捕获会导致空悬指针问题。
- 按值的默认捕获极易受空悬指针影响（尤其是`this`），并会误导人认为lambda表达式是自洽的。



# 条款32，使用初始化捕获将对象移入闭包

初始化捕获，则你有机会指定：

- 由lambda生成的闭包类中的成员变量的名字。
- 一个表达式，用以初始化该变量。

来看一个例子：

```c++
class Widget
{
    public:
  		bool isValidated() const;
    	bool isProcessed() const;
    	bool isArchived() const;
    
    private:
    	
};


auto pw = std::make_unqiue<Widget>();


auto func = [pw = std::move(pw)]{return pw->isValidated() && pw->isArchived();}
```

> `=`左侧的，是你所指定的闭包类成员变量的名字；右侧的是起初始化表达式；而其左右侧属于不同的作用域，左侧的作用域是闭包类的作用域，而右侧则是于lambda表达式加以定义之处的作用域相同。



在C++11中，没有上述的初始化捕获，但是我们可以采用手工模拟的方法：

1. 把需要捕获的对象移动到`std::bind`产生的函数对象中。
2. 给到lambda表达式一个指涉到欲捕获的对象的引用。

```c++
//c++14的写法
std::vector<double> data;
...;			//灌入数据
auto func = [data = std::move(data)]{/*do what you want*/}

//c++11的写法
std::vector<double> data;
...;
auto func = std::bind([](const std::vector<double> & data){/*do what you want*/},std::move(data)
                     );
```

> 和lambda表达式类似，`std::bind`也生成函数对象。我称`std::bind`返回的函数对象为绑定对象，其第一个实参是个可调用值，接下来所有实参表示传给该对象的值。



###### 总结

- 使用C++14的初始化捕获将对象移入闭包。
- 在C++11中，经由手工实现的类或`std::bind`去模拟初始化捕获。



# 条款33，对auto&&型别形参使用decltype，以std::forward之

泛型lambda表达式是C++14中的新特性之一。

```c++
auto f = [](auto x){return func(normalize(x));};

但是如果normalize区别对待左值和右值，那么这种写法是有问题的，lambda表达式总会传递左值给normalize,即使传递给lambda表达式的是一个右值;

我们可以通过万能引用和decltype来实现该功能，如果传入的是一个左值，那么decltype(x)就是一个左值，如果传入的是一个右值，那么decltype(x)就是一个右值;

auto f = [](auto && param){return func(normalize(std::forward<decltype(param)>));}
```



###### 总结

- 对于`auto&&`型别的参数使用`decltype`，以`std::forward`之。



# 条款34，优先选用lambda表达式，而不是std::bind

在c++11中，`std::bind`仅在两个受限的场合还算有着使用的理由：

- **移动捕获**。C++11的lambda表达式没有提供移动捕获特性，但可以通过结合`std::bind`和`lambda`式来模拟移动捕获。
- **多态函数对象**。因为绑定对象的函数调用运算符利用了完美转发，它就可以接受任何型别的实参。

###### 总结

- lambda表达式比起使用`std::bind`而言，可读性更好，表达力更强，可能运行效率也更高。
- 仅在C++11中，`std::bind`在实现移动捕获，或是绑定到具备模板化的函数调用运算符对象的场合中，可能有余热可以发挥。









