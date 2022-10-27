# 智能指针

### shared_ptr

该指针采用的是**共享所有权**来管理所指向对象的生存期。

###### 常规初始化

```c++
shared_ptr<int> pi(new int(100));		//pi指向一个值为100的int型数据
shared_ptr<int> pi2 = new int(100);		//错误，智能指针不接受隐式转换
```



###### make_shared函数

```c++
shared_ptr<int> p2 = make_shared<int>(100);         //int * pi = new int(100);
shared_ptr<string> p3 = make_shared<string>(5,'a');     //5 ge 'a' 字符


shared_ptr<int> p4 = make_shared<int>();        //int 0;

p4 = make_shared<int>(50);          //重新挂到新的对象，释放之前的对象
```



### shared_ptr常用操作、计数与自定义删除器

```c++
auto p6 = std::make_shared<int>(100);
auto p7(p6);		//p7 = p6
```

> 1. 像上面的代码这样，用p6来初始化p7智能指针，就会导致所有指向该对象（内存）的`shared_ptr`引用计数全部增加1。



如果传递引用作为形参进来，则引用计数不会增加

```c++
void myfunc(shared_ptr<int> ptmp)
{
    return;
}

myfunc(p7);		//函数执行完毕后，这个指针的引用计数会恢复
```



```c++
shared_ptr<int> myfunc2(shared_ptr<int> & ptmp)	//这里是引用，所有计数还是2
{
    return ptmp;
}

auto p8 = myfunc2(p7);		//如果有p8接收返回值，此时引用计数会变成3
//但是如果没有对象来接受，引用计数还是2
```

引用计数的减少

（1）新值

```c++
 p8 = make_shared<int>(100);         //p8 指向新对象，计数为1，p6,p7恢复为2
p7 = make_shared<int>(200);         //p7 指向新对象，计数为1，p6恢复为1
```

（2）局部的`shared_ptr`离开其作用域

```c++
auto p6 = make_shared<int>(100);
auto p7(p6);

myfunc2(p7);		//进入函数体有3，返回是2
```

（3）当一个`shared_ptr`引用计数变为0，它会自动释放自动所管理的对象

```c++
auto p9 = make_shared<int>(100);	//只有p9指向对象
auto p10 = make_shared<int>(100);		//
p9 = p10;		//p9会变成2，但是p9原来的对象会被释放
```



### shared_ptr常用操作

`use_count`函数

该成员函数主要是用于返回多少个智能指针指向某个对象。主要是用于调试目的，效率不高。

```c++
shared_ptr<int> myp(new int(100));
int icount = myp.use_count();		//1
shared_ptr<int> myp2(myp);			
icount - myp.use_count();			//2
shared_ptr<int> myp3;
myp3 = myp2;
icount = myp.use_count();			//3
icount = myp3.use_count();			//3
```



`unique`函数

判断该只能指针是否独占，独占返回`true`

```c++
shared_ptr<int> myp(new int(100));
if(myp.unique())
{
   cout << "myp unique ok" << end;
}
```



`reset`函数

（1）当`reset`函数不带参数时

若指针是唯一指向该对象的指针，则释放指针指向的对象，然后将指针置为空

如指针不是唯一指向该对象的指针，则不是放对象，引用计数减一，该指针置为空

```c++
shared_ptr<int> pi(new int(100));
pi.reset();			//第一种情况
if(pi == nullptr)	//满足
{
    cout << "pi is null" << endl;
}
```

（2）当`reset`带参数时

若该指针是唯一指向该对象的指针，则释放该指针指向的对象，让指针指向新内存。

若指针不是唯一指向该对象的指针，则不释放指针指向的对象，但指向该对象的指针引用计数减一，同时让指针指向新对象。

```c++
shared_ptr<int> pi(new int(100));
pi.reset(new int(1));		//释放原内存，指向新内存
```



（3）空指针也可以通过`reset`来重新初始化

```c++
shared_ptr<int> p;				//p 现在是空指针
p.reset(new int(100));			//
```



`*`解引用

获得指针指向的对象



`get()`

返回智能指针中保存的原始指针。

```c++
shared_ptr<int> myp(new int(100));

int * p = myp.get();

*p = 45;		//千万不要delete这个裸指针
```



`swap`函数

用于交换两个指针指针所指向的对象。交换不会引起引用计数的变化

```c++
shared_ptr<string> ps1(new string("I Love China1!"));
shared_ptr<string> ps2(new string("I Love China2!"));
swap(ps1,ps2);
ps2.swap(ps1);
```



`=nullptr`置空

- 将所指向对象的引用计数减1，若引用计数变为0，则释放智能指针所指向的对象
- 将智能指针置空



### 指定删除器

默认的指定删除器就是使用`delete`删除它所指向的对象

```c++
void myDeleter(int * p)
{
    delete p;       //如果不写这句话，还是用自定义删除器，那么就会出现内存泄漏
}

shared_ptr<int> p(new int(12345), myDeleter);
shared_ptr<int> p2(p);              
p2.reset();
p.reset();          

//或者使用lambda表达式的形式

shared_ptr<int> p9(new int(12343), [](int * p){
        delete p;
    });

p.reset();          //同样会调用
```



常规情况下，默认的删除器处理不了动态数组的情况

```c++
shared_ptr<int[]> p(new int[10], [](int * p){
    delete[] p;
});
p.reset();		//不写自定义删除器就有问题
```



使用默认的删除器

```c++
template<typename T>
shared_ptr<T> make_shared_array(size_t size)
{
    return shared_ptr<T>(new T[size], default_delete<T[]>());	
}

shared_ptr<int> pintArr = make_shared_array<int>(5);		//元素个数
```



**就算是两个shared_ptr指定的删除器不相同，只要它们所指向的对象相同，那么这两个shared_ptr的类型也相同**



### weak_ptr

这个智能指针指向一个由`shared_ptr`管理的对象，但是这种智能指针并不控制所指向的对象的生存期。

```c++
auto pi = make_shared<int>(100);
weak_ptr<int> piw(pi);
//piw弱共享pi，pi的引用计数不改变，弱引用计数会从0变成1，两者指向相同位置
```



`use_count`

获取与该弱指针共享对象的其他`shraed_ptr`的数量，或者说获得当前所观测资源的引用计数。

```c++
auto pi = make_shared<int>(100);
auto pi2(pi);
weak_ptr<int> piw(pi);
int isc = piw.use_count();			//2
```



`expired`成员函数

是否过期的意思，若该指针的`use_count`为0，则返回`true`。判断所观测的对象是否被释放。

```c++
pi.reset();
pi2.reset();
if(piw.expired()) {} //成立
```



`reset`函数

将该弱引用指针设置为空，不影响指向该对象的强引用的数量，但指向该对象的弱引用数量会减一

```c++
auto pi = make_shared_ptr<int>(100);
weak_ptr<int> piw(pi);
piw.reset();			//pi是一个强引用，但是此时无弱引用
```



`lock`成员函数

获取所监视的`shared_ptr`。如果强引用指向的对象存在，那么会返回一个`shared_ptr`，并且此时强引用的计数会加一。没有就会返回一个空

```c++
auto p1 = make_shared<int>(100);
weak_ptr<int> pw;
pw = p1;        //一强一弱

if(!pw.expired())
{
    //没过期
    auto p2 = pw.lock();            //2强一弱
    if(p2 != nullptr)
    {
        cout << "所指对象存在" << endl;
    }
    //注意，离开这个范围，p2会被释放，强引用计数减一
}
else {cout << "过期了" << endl;}

p1.reset();

if(pw.expired())
{
    cout << "已经过期了" << endl;
}


{
    auto p1 = make_shared<int>(42);
    pw = p1;
}

if(pw.expired())
{
    cout << "已经过期了" << endl;
}
```



### shared_ptr陷阱分析

###### 慎用裸指针

```c++
void proc(shared_ptr<int> ptr)
{
    return;
}

//main
int * p = new int(100);
//proc(p);		//错误的语法，不能直接隐式转换裸指针
proc(shared_ptr<int>(p));
*p  = 45;			//这里的p指向的对象在上一行已经被释放了

还有另一个情况;
int * pi = new int;
shared_ptr<int> p1(pi);
shared_ptr<int> p2(pi);		//这样会导致p1和p2之间没有关系
```



###### 慎用get返回的指针

```c++
shared_ptr<int> myp(new int(100));
int * p = myp.get();
delete p;		//会导致异常

来看另一种情况，也不能将其他智能指针绑到get返回的指针上。;
shared_ptr<int> myp(new int(100));
int * p = myp.get();
{
    shared_ptr<int> myp2(p);
    //这行代码很微信啊，现在myp和myp2引用计数都为1，但是一旦跳出这个代码块。就会释放两个指针的指的对象。
}
*myp = 100;

//这段代码的问题不是在这个代码块里面，代码块里面会释放一次指针，其实会把内存放掉，但是还是能给*myp = 100赋值，但是最后整个程序结束的时候，尝试释放myp的对象会出现问题。
```



所以永远不要`get`得到的指针来初始化另外一个智能指针或者给另外一个智能指针赋值。



###### 用enable_shared_from_this返回this

```c++
class CT
{
    public:
        shared_ptr<CT> getself()
        {
            return shared_ptr<CT>(this);    //这行代码很危险，类似返回get的指针
        }
};

shared_ptr<CT> pct1(new CT);
shared_ptr<CT> pct2 = pct1;		//这里的没问题，相当于重新构造了一个智能指针

//但是如果修改11行的代码为
shared_ptr<CT> pct2 = pct1->getself();		//这就和传入裸指针是一样的了。

而是应该修改如下:
class CT : public enable_shared_from_this<CT>   //这是一个类模板
{
    public:
        shared_ptr<CT> getself()
        {
            return shared_from_this(); //这个会返回shared_ptr智能指针
        }
};
```

> `enable_shared_from_this`是一个类模板，它的类型模板参数就是继承它的子类的类名。该类模板中有一个弱指针，这个弱指针能够观测`this`，调用`shared_from_this`方法的时候，这个方法内部实际是调用了这个弱指针的`lock`方法，`lock`方法会让`shared_ptr`指针加一，同时返回`shared_ptr`。



###### 避免循环引用

```c++
class CA:
class CB;
class CA
{
    public:
    	shared_ptr<CB> m_pbs;
};

class CB
{
    public:
    	shared_ptr<CA> m_pas;
    	//weak_ptr<CA> m_pas
};

shared_ptr<CA> pca(new CA);
shared_ptr<CB> pcb(new CB);
pca->m_pbs = pcb;
pcb->m_pas = pca;
```

> 这里会导致强引用计数为2，这样的话就是离开计数器的时候各自减1， 没有变成0，所以两者都不会被释放。



###### 移动语义

```c++
shared_ptr<int> p1(new int(100));
shared_ptr<int> p2(move(p1));		//这里是移动，原来的p1内存被移动到了p2,引用计数不会增加
shared_ptr<int> p3;
p3 = move(p2);		//同理
```



### unique_ptr

该智能指针是一种独占式指针，或者理解成专属所有权这种概念也可以。也就是说，同一时刻，只能有一个`unique_ptr`指向这个对象。

```c++
unique_ptr<int> pi;
if(pi == nullptr) {do something;}
unique_ptr<int> pi2(new int(105));		//pi2绑定到105这个对象
```



```c++
unique_ptr<int> p1 = make_unique<int>(100);
auto p2 = make_unique<int>(200);
```



#### 常用操作

```c++
unique_ptr<string> ps1(new string("I Love China!"));
unique_ptr<string> ps2(ps1);		//不可以
unique_ptr<stirng> ps3 = ps1;		//不可以
unique_ptr<string> ps4;
ps4 = ps1;							//不可以
```

> `unique_ptr`不支持复制、赋值等动作，是一个只能移动不能复制的类型。



###### 移动语义

```c++
unique_ptr<string> ps1(new string("I Love China!"));
unique_ptr<string> ps2 = move(ps1);		//移动语义
```



`release`成员函数

放弃对指针的控制权，返回裸指针，将只能指针置空。返回的这个裸指针可以手工`delete`释放，也可以用来初始化另一个智能指针。

```c++
unique_ptr<string> ps1(new string("I Love String"));
unique_ptr<string> ps2(ps1.release());
if(ps1 == nullptr) {do somethings;}

ps2.release();		//这样会导致内存泄漏，必须要有指针出来接
string * temp = ps2.release();
delete temp;
```



`reset`函数

当`reset`不带参数时，释放智能指针指向的对象，并将智能指针置空。当`reset`带参数时，释放智能指针原来指向的内存，指向新的内存。

```c++
unique_ptr<string> prsdc(new string("I Love China1!"));
unique_ptr<string> prsdc2(new string("I Love china2!"));

prsdc2.reset(prsdc.release());
prsdc.reset(new string("I Love China!"));
```



###### 指向一个数组

```c++
unique_ptr<int[]> ptrarray(new int[10]);

ptrarray[0] = 12;
ptrarray[1] = 24;
ptrarray[9] = 123;

cout << ptrarray[3];		//垃圾值
```



转换成`shared_ptr`类型

如果`unique_ptr`为右值，就可以将其赋值给`shared_ptr`。

```c++
auto myfunc()
{
    return unique_ptr<string>(new string("I Love China!")); //临时对象都是右值
}

shared_ptr<string> pss1 = myfunc();     //引用计数加1

unique_ptr<string> ps(new string("I Love China!"));
shared_ptr<string> ps2 = move(ps);
```



`unique_ptr`对象被销毁还是能够复制的，也就是可以做返回值

```c++
unique_ptr<string> tuniqp()
{
    unique_ptr<string> pr(new string("I Love China!"));
    return pr;
    // return unique_ptr<string>(new string("I Love China!"));
}

auto ps2 = tuniqp();
```



##### 删除器

```c++
void UniqueDeleter(string * del)
{
    delete del;
    del = nullptr;
}

using fp = void(*)(string *);
unique_ptr<string, fp> ps1(new string("I Love China!"), UniqueDeleter);

//

unique_ptr<string, decltype(UniqueDeleter)*> ps2(new string("I Love China1"),UniqueDeleter);


//

auto delFunction = [](string * del) {delete del; del = nullptr;};
unique_ptr<string, decltype(delFunction)> ps3(new string("I Love China3"), delFunction);
```



对于`unique_ptr`的尺寸，与裸指针是一样的，但是如果删除器是`lambda`，那么就尺寸不变，如果是自定义函数，那么尺寸就会发生变化。



