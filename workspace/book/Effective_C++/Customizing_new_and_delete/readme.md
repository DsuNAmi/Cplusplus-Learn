# 定制new和delete

C++允许开发人员手工管理内存。可以研究并学习它们的软件使用内存的行为和特征，然后修改分配和归还工作，以求获得其所建置的系统的最佳效率。



**STL容器所使用的heap内存是由容器所拥有的内存分配器对象管理，不是被new和delete直接管理。**



## 了解new-handler的行为

当`operator new`无法满足某一内存分配需求时，它会抛出异常，在抛出异常之前，他会调用一个客户指定的错误处理函数，一个所谓的`new-handler`。为了指定这个用来处理内存不足的函数。，客户必须调用`set_new_handler`，那是声明于<new>的一个标准程序库函数：

```c++
namespace std
{
  	typedef void (*new_handler)();	//指向返回值为空，参数为空的函数。
    new_handler set_new_handler(new_handler p) throw();
    //获得一个new_handler并返回一个new_handler的函数。throw表示一份异常明细，表示该函数不抛出任何函数。
};
```



你可以这样使用`new_handler`：

```c++
void outOfMem()
{
    str:cerr << "Unable to satisfy request for memory\n";
    std::abort();
}

int main()
{
    std::set_new_handler(OutOfMem);
    int * pBigDataArray = new int(100000000L);	//无法分配这么大的内存，但是编译器会报错
}
```



当`operator_new`无法满足内存申请时，他会不断调用`new_handler`函数，直到找到足够内存。

设计一个良好的`new_handler`函数必须做以下事情：

- **让更多内存可被使用**：这便造成`operator new`的下一次内存分配动作可能成功。实现此策略的方法是，程序一开始执行就分配一大块内存，而后当`new_handler`第一次被调用，就将它们释放给程序使用。
- **安装另一个new_handler**：如果目前这个`new_handler`无法取得更多可用内存，或许它知道哪个`new_handler`有这个能力。为达到此目的，做法之一是令`new_handler`修改会影响`new_handler`行为的`static`数据、`namespace`数据、`global`数据。
- **卸除new_handler**：也就是将`nullptr`指针传给`set_new_handler`。一旦没有安装任何`new_handler`，`operator new`会在内存分配不成功时抛出异常。
- **抛出bad_alloc的异常**：这样的异常不会被`operator new`捕捉，因此会被传播到内存索求处。
- **不返回**：通常调用`abort()`或者`exit()`函数。

来看一个例子：

```c++
有时候你希望以不同的方式处理内存分配失败的情况;
class X
{
  	public:
    	static void outOfMemory();
    	...;
};

class Y
{
  	public:
    	static void outOfMemory();
    	...;
};

X * p1 = new X;					//分配不成功调用X::outOfMemory()
Y * p2 = new Y;					//同理
```



C++并不支持类专属`new_handler`，但其实也不需要。但是也可以自己实现，来看一个类专属的`new_handler`例子：

```c++
class Widget
{
  	public:
		static std::new_handler set_new_handler(std::new_handler p) throw();
    	static void* operator new(std::size_t size) throw(std::bad_alloc);
    private:
    	static std::new_handler currentHandler;		//除了整数型，其他都必须在类外定义
};

std::new_handler Widget::currentHandler = nullptr;//0;

std::new_handler set_new_handler(std::new_handler p) throw()
{
    std::new_handler oldHandler = currentHandler;
    currentHandler = p;
    return oldHandler;
}

class NewHandlerHolder
{
  	public:
    	explicit NewHandlerHolder(std::new_handler nh)
            : handler(nh) {}		//取得目前的new-handler
    	~NewHandlerHolder() {std::set_new_handler(handler);}//释放
    private:
    	std::new_handler handler;		//记录
    	NewHandlerHolder(const NewHandlerHolder &); //阻止复制
    	NewHandlerHolder & operator=(const NewHandlerHolder &);
};

void * operator new(std::size_t size) throw(std::bad_alloc)
{
    NewHandlerHolder h(std::set_new_handler(currentHandler));
    return ::operator new(size);
}

//调用
void OutOfMem();		//函数声明，此函数在Widget分配内存失败时调用

Widget::set_new_handler(outOfMem);		//设定new-handler函数

Widget * pw1 = new Widget;			//分配失败调用

std::string * ps = new std::string;			//如果内存分配失败，调用global new-handler函数

Widge::set_new_handler(0);				//设定专属的为nullptr;

Widget * pw2 = new Widget;				//此时分配内存失败直接抛出异常。
```



最后，`Widget`的`operator new`会做以下事情：

- 调用标准`set_new_handler`，告知`Widget`的错误处理函数。这会将`Widget`的`new_handler`安装为`global new-handler`。
- 调用`global operator new`，执行实际内存分配。如果分配失败，`global operator new`会调用`Widget`的`operator new`，因为那个函数才刚被安装为`global new-handler`。如果`global operator new`最终无法分配内存，会抛出`bad_alloc`异常。在此情况下`Widget`的`operator new`必须恢复原本的`global new-handler`，然后再传播该异常。为确保原本的`new-handler`总是能被重新安装回去，`Widget`将`global new-handler`视为资源并防止资源泄露。
- 如果`global oprator new`能够分配足够一个`Widget`对象所用的内存，`Widget`的`operator new`会返回一个指针，指向分配所得。`Widget`析构函数会管理`global new-handler`，它会自动将`Widget's operator new`被调用的那个`global new handler`恢复回来。



==set_new_handler允许客户指定一个函数，在内存分配无法获得满足时被调用==

==Nothrow new是一个颇为局限的工具，因为它只适用于内存分配；后继的构造函数调用还是可能抛出异常==





## 了解new和delete的合理替换时机

为什么会有人想要替换编译器提供的`operator new`或者是`operator delete`呢？

- 用来检测运用上的错误。
- 为了强化效能。
- 为了收集使用上的统计数据。
- 为了增加分配和归还的速度。
- 为了降低缺省内存管理器带来的额外空间开销。
- 为了弥补缺省分配器中的非最佳齐位。
- 为了将相关对象成簇集中。
- 为了获得非传统的行为。

==有许多理由需要写个自定义的new和delete，包括改善效能，对heap运用错误进行调试，收集heap信息。==



## 编写new和delete时需要固守常规

`operator new`实际上不止一次尝试分配内存，并且在每次失败后调用`new-handler`函数。

而C++规定，即使客户要求0Bytes，`operator new `也得返回一个合法的指针。

```c++
//new 

void* operator new(std::size_t size) throw(std::bad_alloc)
{
    using namespace std;
    if(size == 0) size = 1;
    while(true)
    {
        //尝试分配内存
        if(分配成功)
        {
            return (一个指向分配得来的内存);
        }
        
        //分配失败：找出目前的new-handler函数
        new_handler globalHandler = set_new_handler(0);
        set_new_handler(globalHandler);
        
        if(globalHandler) (*globalHandler)();
        else throw(std::bad_alloc);
    }
}

//delete
void operator delete(void * rawMemory) throw()
{
    if(rawMemory == 0) return;		//被删除的是空指针那么就什么都不做
    
    //归还内存
}
```



==operator new应该内含一个无穷循环，并在其中尝试分配内存，如果它无法满足内存需求，就该调用new_handler。它也应该有能力处理0Bytes申请。类专属版本则还应该处理 比正确大小更大的错误申请。==

==operator delete应该在收到nullptr时不做任何事。类专属版本还应该处理比大小更大的错误申请==



## 写了placement new 也要写placement delete

```c++
Widget * pw = new Widget;
//两件事
1.operator new;
2.Widget构造函数。;

void* operator new(std::size_t, void* pMemory) throw();
```



如果接受的参数除了一定会有的那个size_t还有其他，那就是一个`placement new`。



==当你写一个placement operator new,请确定也写出了对应的placement delete。如果没有这样做，你的程序可能会发生隐微而时断时续的内存泄露==

==当你声明了placement new，placement delete，请确定不要无意识地遮掩了它们的正常版本。==