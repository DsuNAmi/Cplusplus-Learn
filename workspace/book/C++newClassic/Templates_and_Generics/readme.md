# 模板和泛型

### 函数模板的定义

```c++
template<typename T>
T funcadd(T a, T b)
{
    T addhe = a + b;
    return addhe;
}
```

### 函数模板的调用

```c++
int he = funcadd(3,1);

float he = funcacc(3,1.2f);			//错误
```



### 非类型模板参数

```c++
template<int a, int b>
int funcaddv2()
{
    int addhe = a + b;
    return addhe;
}

//调用方法
int result = funcaddv2<12,13>();

//错误列子
int a = 12;
int result = funcaddv2<a,13>();			//这是错误方法

//另一种定义
template<typename T, int a, int b>
int funcaddv3(T c)
{
    int addhe = (int)c + a + b;
    return addhe;
}

//调用
int result = addhe<double, 12, 13>(13);		//系统会传一个double而不是一个int，以<>传递的类型为准
```



### 类模板的定义

```c++
template<typename T>
class myvector
{
    public:
    	typedef T * myiterator;
    	myvector();
    	myvector & myvector=(const myvector&);
    	myiterator mybegin();
    	myiterator myend();
    	void myfunc();
};

//定义对象
myvector<int> tmpvec;		//T被替换成int
```



### 类模板的成员函数

```c++
//在类外定义
template<typename T>
void myvector<T>::myfunc(){}

template<typename T>
myvector<T>::myvector(){}
```

> 一个实例化的模板，它的成员只有在使用的时候才会被实例化。



### 非类型模板参数的使用

```c++
template<typename T, int size = 10>
class myarray
{
    private:
    	T arr[size];
   	public:
    	void myfunc();
};

template<typename T, int size>
void myarray<T,size>::myfunc()
{
    std::cout << size << std::endl;
}

myarray<int, 10> tmparr;
```

> 浮点型一般不能作为非模板参数。
>
> 类类型也不能作为非类型模板参数。



### typename的用法

`typename`的另一个用法是通知编译器某个名字代表的是一个类型。但是这里的`typename`不能用`class`代替。

```c++
template<typename T>
typename T::size_type GetLength(const T & c)
{
    if(c.empty())
    {
        return 0;
    }

    return c.size();
}
```



### 函数指针作为其他函数的参数

```c++
typedef int(*funType)(int,int);

int mf(int t1, int t2)
{
    return 1;
}

void testfunc(int i, int j, funType funcpoint)
{
    cout << funcpoint(i,j) << endl;
}
```



如果涉及到模板：

```c++
template<typename T, typename F>
void testfunc(const T & i, const T & j, F funcpoint)
{
    cout << funcpoint(i,j) << endl;
} 
```



来看一个例子：

```c++
class tc
{
    public:
        tc() {cout << "ctor" << endl;}
        tc(const tc & t) {cout << "copy ctor" << endl;}
        int operator()(int v1, int v2) {return v1 +v2;}
};

int mf(int t1, int t2)
{
    return 1;
}

// void testfunc(int i, int j, funType funcpoint)
// {
//     cout << funcpoint(i,j) << endl;
// }


template<typename T, typename F>
void testfunc(const T & i, const T & j, F funcpoint)
{
    cout << funcpoint(i,j) << endl;
} 

//调用
tc tcobj;
testfunc(3,4,tcobj);      //调用copy ctor

testfunc(3,4,tc());      //调用ctor
```



### 默认模板参数

###### 类模板

```c++
template<typename T = string, int size = 5>
class myarray{};

//调用
myarray<> tmpa;		//默认的是string类型
myarray<int,10> tmpi;		//
```



###### 函数模板

```c++
template<typename T, typename F = tc>
void testfunc(const T & i, const T & j, F funcpoint = F())
{
    cout << funcpoint(i,j) << endl;
}
```

> 必须同时为模板参数和函数参数指定默认值，一个也不能少，否则语法通不过且语义也不完整。



### 成员函数模板，模板显式实例化与声明

```c++
template<typename C>
class A
{
    public:
        template<typename T2>
        A(T2 v1, T2 v2);

        template<typename T>
        void myft(T tmpt){cout << tmpt << endl;}

    C m_ic;
};

template<typename C>
template<typename T2>
A<C>::A(T2 v1, T2 v2)
{
    cout << v1 << v2 << endl;
}

```

> 类模板中的成员函数，只有源程序代码中出现调用这些成员函数代码时，这些成员函数才会出现在一个实例化了的类模板中。
>
> 类模板中的成员函数模板，只有源程序代码中出现调用这些成员函数模板的代码时，这些成员函数模板的具体实例才会出现在一个实例化了的类模板中。



### using定义模板别名与显式指定模板参数

```c++
template<typename T>
using str_map_t = std::map<std::string, T>;

using Funtype = int(*)(int,int);		//定义函数指针别名
```



### 模板全特化和偏特化

#### 类模板全特化

**必须先有泛化版本才有特化版本**

```c++
//generics
template<typename T, typename U>
struct TC
{
    TC()
    {
        cout << "TC generics ctor" << endl;
    }
    void functest()
    {
        cout << "TC functest generics" << endl;
    }
};

//all
template<>
struct TC<int,int>
{
    TC()
    {
        cout << "TC<int, int> all ctor" << endl;
    }
    void functest()
    {
        cout << "TC<int, int> all " << endl;
    }
};

template<>
struct TC<double,int>
{
    TC()
    {
        cout << "TC<double,int> all ctor" << endl;
    }
    void functest()
    {
        cout << "TC<double,int> all " << endl;
    }
};


TC<char,char> tcchar;
tcchar.functest();
TC<int,int> tcint;
tcint.functest();
TC<double,int> tcdoubleint;
tcdoubleint.functest();
```

#### 类模板偏特化

```c++
template<typename T, typename U, typename W>
struct TCP
{
    TCP()
    {
        cout << "TCP generics ctor" << endl;
    }

    void functest()
    {
        cout << "TCP generics functest" << endl;
    }
};

//part
template<typename C>    //这个名字不一定要相同的
struct TCP<int, C, double>
{
    TCP()
    {
        cout << "TCP<int,C,double> ctor" << endl;
    }

    void functest()
    {
        cout << "TCP<int,C,double> functest" << endl;
    }
};

TCP<double,int,double> tcpdi;
tcpdi.functest();
TCP<int,int,double> tcpii;
tcpii.functest();
```



### 模板参数的偏特化

```c++
template<typename T>
struct TCF
{
    TCF()
    {
        cout << "TCF generics ctor" << endl;
    }
    void functest()
    {
        cout << "TCF generics functest" << endl;
    }
};

template<typename T>
struct TCF<const T>
{
    TCF()
    {
        cout << "TCF<const T>  ctor" << endl;
    }
    void functest()
    {
        cout << "TCF<const T> functest" << endl;
    }
};

TCF<double> tcfd;
tcfd.functest();
TCF<const double> tcfcd;
tcfcd.functest();
```



### 函数模板特化

```c++
template<typename T, typename U>
void tfunc(T & tmprv, U & tmprv2)
{
    cout << "tfcun generics version" << endl;
    cout << tmprv << endl;
    cout << tmprv2 << endl;
}

//all
template<>
void tfunc(int & tmprv, double & tmprv2)
{
    cout << "tfcun<int,double> version" << endl;
    cout << tmprv << endl;
    cout << tmprv2 << endl;
}
//function 
const char * p = "I Love China";
int i = 12;
tfunc(p,i);

int k = 12;
double db = 15.8f;
tfunc(k,db);
```



函数模板不能偏特化，会报错



### 可变参数和模板模板参数

```c++
template<typename... T>
void myfunct(T... args)
{
    cout << sizeof...(args) << endl;        //sizeof... 属于固定语法
    cout << sizeof...(T) << endl;
}

template<typename T, typename... U>
void myfunct2(const T & firsttag, const U & ... othertags)
{
    cout << sizeof(firsttag) << endl;
    cout << sizeof...(othertags) << endl;       //这两个sizeof含义不同
}

//multi template
myfunct();
myfunct(10,20);
myfunct(10,25.6,"fdsf");


cout << "-----" << endl;
// myfunct2();     //错误语法，第一个参数必须要有
myfunct2(10);
myfunct2(10,25.6,"fadsf");
```

> 一般把上面的`args`称为一包或者一堆参数，而且每个参数的类型可以各不相同，所以理解T这个名字的时候，不能把它理解成一个类型，而是要理解成0到多个不同的类型。
>
> 这包参数钟可以容纳0个到多个模板参数，而且这些模板参数可以为任意的类型。
>
> ...表示多个类型。



###### 如何展开参数包？

要求在代码编写中有一个参数展开函数和一个同名的递归终止函数，通过这两个函数把参数包展开。

```c++
//另一个同名的终止函数
void OpenArgs() //没有参数的时候停止
{
    cout << "参数包展开时执行了递归终止函数" << endl;
}


//参数展开
template<typename T, typename... U>
void OpenArgs(const T & firstarg, const U & ... otherargs)
{
    cout << "收到的参数为:" << firstarg << endl;
    OpenArgs(otherargs...);
}
```



### 可变参类模板

```c++
template<typename ...Args> //特化要写一个泛化的版本
class myclasst
{
    public:
        myclasst()
        {
            printf("myclasst::myclasst()泛化版本执行了,this=%p,sizeof...(Args) = %d\n",this,sizeof...(Args));
        }
};


template<typename First, typename ... Others>
class myclasst<First, Others...> : private myclasst<Others...>  //偏特化，这里继承的是泛化版本的
{
    public:
        myclasst() : m_i(0)
        {
            printf("myclasst::myclasst() 偏特化版本执行了, this=%p, sizeof...(Others) = %d\n",this,sizeof...(Others));
        }
    

    First m_i;
};


myclasst<int,float,double> myc;

//输出
myclasst::myclasst()泛化版本执行了,this=000000f0d17ff870,sizeof...(Args) = 0
myclasst::myclasst() 偏特化版本执行了, this=000000f0d17ff870, sizeof...(Others) = 0
myclasst::myclasst() 偏特化版本执行了, this=000000f0d17ff870, sizeof...(Others) = 1
myclasst::myclasst() 偏特化版本执行了, this=000000f0d17ff870, sizeof...(Others) = 2
```

>观察输出，执行代码`myclasst<int,float,double> myc`时，系统会去实例化的是三个类型模板参数的类模板。根据写法`class myclasst<First, Other...> : private myclasst<Other...>`，它继承的是两个类型模板参数的类，而两个类型模板参数的类模板继承的是一个类型模板参数的类模板，因此，系统首先会去实例化带一个类型模板参数的类模板，然后实例化带两个类型模板参数的类模板，最后实例化带三个类型模板参数的类模板。



所以继承的方式应该是：

`myclasst<>` -> `myclasst<double>` -> `myclasst<float, double>` -> `myclasst<int,float,double>`

所以一共是进行了四次构造函数。

同时我们也可以自己定义一个空特化的版本

```c++
template<>
class myclasst<>
{
    public:
        myclasst()
        {
            printf("myclasst<>::myclasst()泛化版本执行了,this=%p\n",this);
        }
};

//修改之后的输出
myclasst<>::myclasst()泛化版本执行了,this=000000f6489ff7d0
myclasst::myclasst() 偏特化版本执行了, this=000000f6489ff7d0, sizeof...(Others) = 0
myclasst::myclasst() 偏特化版本执行了, this=000000f6489ff7d0, sizeof...(Others) = 1
myclasst::myclasst() 偏特化版本执行了, this=000000f6489ff7d0, sizeof...(Others) = 2
```



可以看到输出结果也变了。



### 模板模板参数



