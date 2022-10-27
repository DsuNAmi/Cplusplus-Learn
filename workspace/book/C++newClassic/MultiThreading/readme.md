# 并发与多线程

来看一个创建线程的例子：

```c++
#include <iostream>
#include <thread>

using namespace std;


void myprint()
{
    cout << "我的线程开始执行了" << endl;
    //
    cout << "我的线程执行完毕了" << endl;
    return;
}

int main()
{
    thread mytobj(myprint);     //创建一个线程，参数是一个函数名，这里会立即执行该函数
    mytobj.join();              //join会卡在这里，等待myprint线程执行完毕，程序流程结束才会往下走,注释掉会出异常

    cout << "main程序执行结束" << endl;
    
    return 0;
}
```



**书写一个良好的程序，应该是主线程等待子线程执行完毕后，自己才能最终退出**。



`detach`字面意思是**分离**的意思，所谓分离，就是主线程不和子线程汇合了，主线程执行主线程的，子线程执行子线程的，之线程不必等子线程运行结束，可以先执行结束，这并不影响子线程的执行。

```c++
myobj.detach();			//不能同时使用detach和Join
```



`joinable`

判断是否可以成功使用`join`或者`detach`

```c++
thread mytobj(myprint);

if(mytobj.joinable())
{
    cout << "l : joinable() == true" << endl;           //chengli
}
else
{
    cout << "l : joinable() == false" << endl;
}

mytobj.join();

if(mytobj.joinable())
{
    cout << "l : joinable() == true" << endl;
}
else
{
    cout << "l : joinable() == false" << endl;       //chengli
}
```



###### 用类来创建线程

```c++
class TA
{
    public:
        void operator()()
        {
            cout << "TA::operator()开始执行了" << endl;
            //
            cout << "TA::operator()执行结束了" << endl;
        }
};

TA ta;
thread mytobj(ta);
mytobj.join();

cout << "main函数执行结束" << endl;
```



但是使用`detach`可能会存在问题

```c++
class TA
{
    public:
        int & m_i;
        TA(int & i) : m_i(i) {}
        void operator()()
        {
            
            cout << "mi1的值为" << m_i << endl;     //可能m_i没有有效值
            cout << "mi2的值为" << m_i << endl;
            cout << "mi3的值为" << m_i << endl;
            cout << "mi4的值为" << m_i << endl;
            cout << "mi5的值为" << m_i << endl;
            cout << "mi6的值为" << m_i << endl;
            cout << "mi7的值为" << m_i << endl;
            cout << "mi8的值为" << m_i << endl;
        }
};

int myi = 6;
TA ta(myi);
thread mytobj3(ta);
mytobj3.detach();
cout << "main函数执行完毕" << endl;
```

> 这里如果主线程结束，因为类中的对象是一个引用，如果继续使用已经销毁的对象，就会出现问题。
>
> 执行到线程中的对象是复制进去的，子线程会复制这个对象，所以主线程结束了，类的对象不会有问题，而是其中的引用所引用的对象已经销毁，这样就有问题。



###### 利用lambda创建线程

```c++
auto mylamthread = [](){
        cout << "我们的线程开始了" << endl;
        cout << "我们的线程完毕了" << endl;
    };


thread mytobj(mylamthread);
mytobj.join();
cout << "main主函数执行结束" << endl;
```



### 线程传递详解、detach坑与成员函数作为线程函数

###### 传递临时对象作为线程对象

```c++
void myprint(const int & i, char * pmybuf)
{
    cout << i << endl;
    cout << pmybuf << endl;
    return;
}


int mvar = 1;
int & mvary = mvar;
char mybuf[] = "this is a best!";
thread mytobj(myprint, mvar, mybuf);
mytobj.join();

cout << "main函数执行完毕" << endl;
```

> 问题1：
>
> 如果要把`join`换成`detach`，那么程序可能出现问题。
>
> 因为`mybuf`是主线程中的内存，如果主线程退出了，就会出现问题。
>
> 所以如果是使用`detach`， 就不要往里面传递指针或者是引用
>
> 问题2：
>
> 如果我们这么改造：
>
> ```c++
> void modiMyPrint(int i, const string & pmybuf)
> {
>     //这里会为const对象创建一个临时对象，即使是引用
>     cout << i << endl;
>     const char * ptmp = pmybuf.c_str();
>     cout << pmybuf.c_str() << endl;
>     return;
> }
> 
> thread mytobj(myprint, mvar, mybuf);
> 
> ```
>
> 这段代码的问题就是`mybuf`在什么时候转换成`string`类型，如果主线程结束了再转换就会出现问题。



结论：

- 如果传递`int`这种简单类型参数，建议都是用值传递，不要使用引用类型，以免节外生枝。
- 如果传递类对象作为参数，则避免隐式类型转换，全部都在船舰线程这一行就构建出临时对象，然后线程入口函数的形参位置使用引用来作为形参。
- 建议不使用`detach`，只使用`join`，就不存在局部变量失效导致线程对内存非法引用的问题。



###### 传递类对象与智能指针作为线程参数

临时对象不能作为非`const`引用参数，也就是必须加`const`修饰，这是因为C++编译器的语义限制。如果一个参数是以非`const`引用传入，C++编译器就有理由认为程序员会在函数中修改这个对象，并且这个被修改的引用在函数后要发挥作用。但如果把一个临时对象当作非`const`参数传进来，由于临时对象的特殊性，程序员并不能操作临时对象，而且临时对象可能随时会被释放掉，所以，一般来说，修改一个临时对象毫无意义。所以，C++编译器加入了临时对象不能作为非`const`引用的语义限制。



`std::ref`，这个函数会把主线程的参数作为引用传递给子线程，而不是像往常一样，子线程不管是否是引用，都会复制一个，使用`std::ref`，就会传递主线程的原始数据，也能够在子线程中修改该对象的值。



如果是使用指针指针作为参数传递，那么就是需要使用`std::move`

```c++
class A
{
    public:
        A(int num) : m_i(num) {}
        A(const A & a) {}
        ~A() {}
        void thread_work(int num)
        {
            cout << "subthread thread_work exec, this = " << this << " threaid = " << this_thread::get_id() << endl;
        }
    int m_i;
};


注意传递的参数;
A myobj(10);
thread mytobj(&A::thread_work, myobj, 15);
mytobj.join();
```



### 创建多个线程、数据共享问题分析与案例代码

###### 创建和等待多个线程

```c++
void myprint(int num)
{
    cout << "thread worked threadId = " << num << endl;
    cout << "thread overed threadID = " << num << endl;
}

vector<thread> mythreads;

for(int i = 0; i < 5; ++i)
{
    mythreads.push_back(thread(myprint, i));
}

for(auto iter = mythreads.begin(); iter != mythreads.end(); ++iter)
{
    iter->join();
}

cout << "main函数执行完毕" << endl;

输出;

thread worked threadId = thread worked threadId = 1thread worked threadId = 3
thread overed threadID = 3
thread worked threadId = 4
thread overed threadID = 4
0
thread overed threadID = 0
thread worked threadId = 2
thread overed threadID = 2

thread overed threadID = 1
main函数执行完毕
```

> - 多个线程之间的执行顺序是乱的。先创建的线程也不见得就一定比后创建的线程执行得快，这个与操作系统内部对线程得运行调度机制有关。
> - 主线程是等待所有子线程运行结束，最后主线程才结束，所以笔者推荐`join`，因为这种写法写出来的多线程程序更容易写得更稳定、健壮。
> - 把`thread`对象放到容器里进行管理，看起来像一个`thread`对象数组，这对一次性创建大量得线程并对这些线程进行管理是很方便的。



###### 数据共享问题分析

**只读的数据**

每个线程读到的数据都是一样的

**有读有写**

读的时候不能写，写的时候不能读

**共享数据实例分析**

代码文件：`netGameSim.cpp`



###### 互斥量

`lock, unlock`的使用规则：

**成对使用**，有lock必须要使用unlock，没调用一次lock，必然要调用一次unlock。主要注意条件分支的时候，每一个出口都要调用一个`unlock()`。

但是C++有智能解锁器，`std::lock_guard`

```c++
bool outMsgLULProc(int & command)
{
    std::lock_guard<std::mutex> sbgguard(myMutex);
    // myMutex.lock();
    if(!msgRecvQueue.empty())
    {
        command = msgRecvQueue.front();
        msgRecvQueue.pop_front();
        // myMutex.unlock();
        return true;
    }
    // myMutex.unlock();       //这里是两个出口，不然条件不满足就不会放锁了
    return false;
}
```

> 在函数结束的时候，也就是`sbguard`作用域效果结束后，就会自动调用`unlock`。
>
> 其实就是构造函数调用`lock`，析构函数调用`unlock`。





###### 死锁演示

```c++
void inMsgRecvQueue()
{
    for(int i = 0; i < 10000; ++i)
    {
        cout << "inMsgRecvQueue()执行";
        myMutex.lock();
        //some data to protect
        myMutex2.lock();
        msgRecvQueue.push_back(i);
        myMutex2.unlock();
        myMutex.unlock();
	}
}

bool outMsgLULProc(int & command)
{
    myMutex2.lock();
    myMutex.lock();
    if(!msgRecvQueue.empty())
    {
        command = msgRecvQueue.front();
        msgRecvQueue.pop_front();
        myMutex.unlock();
        myMutex2.unlock();
        return true;
    }
    myMutex.unlock();
    myMutex2.unlock();
    return false;
}
```

> 上述代码会互相等待导致死锁。

`std::lock`函数模板能够一次锁住多个互斥量，比如要锁两个互斥量，第一个锁住了，第二个没锁住，那么就会放掉第一个。不会导致死锁。

```c++
std::lock(myMutex, myMutex2);	//但是还是要自己解锁

myMutex.unlock();
myMutex2.unlock();
//此时顺序并没关系

std::lock_guard<std::mutex> sbguard1(myMutex, std::adopt_lock);
//第二个参数主要是告诉程序，这个互斥量已经锁过了，不需要再上锁。
//使用的前提就是要提前把互斥量锁上
```



###### unique_lock

查看源代码`netGameSim.cpp`

锁的粒度：**锁住代码的多少**，一般用粗细表示

- 锁住的代码少，粒度就细，程序执行效率就高。
- 锁住的代码多，粒度就粗，程序执行效率就低。



###### 单例

查看源代码：`singleton.cpp`



###### std::call_once

保证某个函数只被调用一次。这个函数保证了，即使是在多线程的情况下，也只会被调用一次。

`std::call_once`就是通过`std::once_flag`标记来判断对应的函数是否执行了。



`future`和`shared_future`

`future.get()`是移动语义

`shared_future.get()`是复制操作。



###### 关于原子操作

`std::atmoic<int>` 并不是所有的操作都是原子操作的。

