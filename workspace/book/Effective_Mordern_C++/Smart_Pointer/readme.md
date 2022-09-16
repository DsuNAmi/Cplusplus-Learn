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

