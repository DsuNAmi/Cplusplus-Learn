# 微调

# 条款41，针对可复制的形参，在移动成本低并且一定会被复制的情况下，考虑将其按值传递

有些函数的形参本来就是打算拿来复制的（这里的意思是将其作用复制或移动操作的源。）

```c++
//重载
class Widget
{
    public:
    	void addName(const std::string & newName)
        {names.push_back(newName);}
    	void addName(std::string && newName)
        {names.push_back(std::move(newName));}
   	private:
    	std::vector<std:string> names;
};

另一种写法是使用万能引用;
class Widget
{
  	public:
    	template<typename T>
    	void addName(T && newName){
            names.push_back(std::forward<T>(newName));
        }
    private:
    	std::vector<std::string> names;
};
```



如果有一种方法来撰写上述函数，针对左值实施的是复制，针对右值实施的是移动，并且只需要维护一个函数，并且不会出现万能引用重载问题。可以采用**按值传递的方式**。

我们可以先来看看其实现方式

```c++
class Widget
{
  	public:
    	void addName(std::string newName)
        {names.push_back(std::move(newNmae));}	//既接收左值，也接收右值，对右值实施移动。
};

Widget w;


std::string name("Bart");

w.addName(name);				//在调addName时传入左值

w.addName(name + "Jenne");		//传入右值
```



我们来看看这三种方法的区别：

- **重载**：无论传入左值还是右值，调用方的实参都会绑定到名字为`newName`的引用上。而这样做不会在复制和移动时带来任何成本。在接收左值的重载版本中，`newName`被复制入`Widget::names`；在接收右值的重载版本中，`newName`被移入`Widget::names`。成本合计：对于左值是一次复制，对于右值是一次移动。
- **使用万能引用**：与重载相同，调用方的实参会绑定到引用`newName`上。由于使用了`std::forward` ，左值`std::string`实参被复制入`Widget::names`中，而右值`std::string`则被移入。成本合计和重载相同。
- **按值传递**：无论传入的是左值还是右值，针对形参`newName`都必须实施一次构造。如果传入的是个左值，成本是一次复制构造。如果传入的是一个右值，成本是一次移动构造。在函数体内，`newName`需要无条件的移入`Widget::names`。成本合计：对左值而言，是一次复制加一次移动，对右值而言，是两次移动。



那么为什么要在某些情况下使用按值传递呢？

1. 它只要求撰写单个函数。能够避免万能引用带来的重载问题。

2. 仅对于可复制的形参，才考虑按值传递。不符合这个要求的形参必然具备只移型别，因为如果它们本来不可复制，但函数却总会创建副本的话，那么就必须经由移动构造函数来创建该副本。如果是只移型别，其复制构造函数被禁用了，所以只需要写一个右值的重载版本就行了，根本不必为了去写按值传递的方法。

   来看一个例子：

   ```c++
   class Widget
   {
     	public:
       	void setPtr(std::unique_ptr<std::string> && ptr)
           {p = std::move(ptr);}
      	private:
       	std::unqiue_ptr<str::string> p;			//只移型别
   };
   
   Widget w;
   
   w.setPtr(std::make_unique<std::string>("Modern C++"));
   
   如果换成按值传递，那么就是两次移动操作，成本就比现在这样使用右值多一次移动。;
   ```

3. 按值传递仅在形参移动成本低廉的前提下，才值得考虑。只有当移动成本低廉时，一次移动带来的额外成本才可能是可以接收的，但是如果这两个前提都不成立，那么执行不必要的移动和执行不必要的复制就没有区别了。

   ```c++
   class Widget
   {
     	public:
       	void addName(std::string newName)
           {
               if((newName.length() >= minLen) && 
                  (newName.lenght() <= maxLen))
               {
                   names.push_back(std::move(newName));
               }
           }
      	private:
       	std::vector<str::string> names;
   };
   ```





###### 总结

- 对于可复制的，在移动成本低廉的并且一定会被复制的形参而言，按值传递可能会和按引用传递的具备相近的效率，并可能生成更少量的目标代码。
- 经由构造复制形参的成本可能比经由复制复制形参高出很多。
- 按值传递肯定会导致切片问题，所以基类型别特别不适用于按值传递。



# 条款42，考虑置入而非插入

我们先来看一段代码：

```c++
std::vector<std::string> vs;			//持有std::string 型别对象的容器

vs.push_back("xyzzy");					//添加字符串字面量


然后我们看看push_back的源码;
template<class T,
		 class Allocator = allocator<T>>
class vector{
  	public:
    	...;
    	void push_back(const T& x);		//插入左值
    	void push_back(T&& x);			//插入右值
};

所以，其实"xyzzy"会被认为是const char[6];
之后代码应该被认为：;
vs.push_back(std::string("xyzzy"));
```

> 性能分析：
>
> 1. 从字符串字面量`xyzzy`出发，创建`std::string`型别的临时对象。该对象没有名字，我们称其为`temp`。针对`temp`实施的构造，就是第一次的`std::string`构造函数的调用。因为是个临时对象，所有`temp`是个右值。
> 2. `temp`被传递给`push_back`的右值重载版本，在那里它被绑定到右值引用形参`x`。然后，会在内存中为`std::vector`构造一个`x`的副本。这一次的构造结果就是在`std:vector`内部创建了一个新的对象（用与将`x`复制到`std::vector`中的构造函数，是移动构造函数，因为作为右值引用的`x`）
> 3. 紧接着`push_back`返回的那一刻，`temp`就被析构了，所以，这就需要调用一次`std::string`的析构函数。



为了解决上述的性能问题，应该采用`emplace_back`。**它使用传入的任何实参在std::vector内构造一个std::string，不会涉及任何临时对象**

`emplace_back`使用了完美转发，所以只要你没有遭遇完美转发的限制，就可以通过`emplace_back`传递任意型别的任意数量和任意组合的实参。

```c++
vs.emplace_back(50,'x');			//插入一个由50个x字符组成的consisting std::string 型别对象
```



我们来看看什么情况置入比插入更高效：

- **欲添加的值是以构造而非赋值方式加入容器**。
- **传递的实参型别与容器持有之物的型别不同**。
- **容器不太可能由于出现重复情况而拒绝待添加的新值**。



##### 但是使用插入和置入还有一个关键问题，和资源管理有关

我们来看一个带自定义删除器的智能指针：

```c++
void killWidget(Widget * pWidget);

std::list<std::shared_ptr<Widget>> ptrs;

ptrs.push_back(std::shared_ptr<Widget>(new Widget, killWidget));
//或者是，两者意义相同
ptrs.push_back({new Widget, killWidget});
```

> 我们来看看上述代码的事件序列：
>
> 1. 上述两个调用语句无论哪个都会构造一个`std::shared_ptr<Widget>`型别的临时对象，用以持有从`new Widget`返回的裸指针。该对象暂时称为`temp`。
> 2. `push_back`会按引用方式接受`temp`。在为链表节点分配内存以持有`temp`的副本过程中，抛出了内存不足的异常。
> 3. 该异常传播到了`push_back`之外，`temp`被析构。作为给`Widget`兜底的、指涉到它并对其施加管理的`std::shared_ptr<Widget>`型别对象会自动释放该`Widget`，在本例的情况下会调用`killWidget`达成目的。

但是如果是：

```c++
ptrs.emplace_back(new Widget, killWidget);
```

> 1. 从`new Widget`返回的指针被完美转发，并运行到`emplace_back`内为链节点分配内容的执行点。然后分配内存失败，并抛出了内存不足的异常。
> 2. 该异常传播到了`emplace_back`之外，作为唯一可以获取堆上`Widget`的抓手裸指针，却丢失了。那个`Widget`都发生了资源泄露。



所以正确的写法如下：

```c++
//push_back
std::shared_ptr<Widget> spw(new Widget, killWidget);
ptrs.push_back(std::move(spw));

//emplace_back
std::shared_ptr<Widget> spw(new Widget, killWidget);
ptrs.emplace_back(std::move(spw));
```



###### 总结

- 从原理上说，置入函数应该有时比对应的插入函数高效，而且不应该有更低效的可能。
- 从实践上说，注入函数在以下几个前提成立时，既有可能会运行的更快；（1）待添加的值是以构造而非赋值方式加入容器；（2）传递的实参型别与容器持有之物的型别不同；（3）容器不会由于存在重复值而拒绝待添加的值。
- 置入函数可能会执行在插入函数中会被拒绝的型别转换。

