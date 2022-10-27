#include <iostream>
#include <string>

using namespace std;

template<int a, int b>
int funcadd()
{
    return a + b;
}


template<typename T, int size>
class MyArray
{
    private:
        T arr[size];
    public:
        void func();
};

template<typename T, int size>
void MyArray<T,size>::func()
{
    cout << size << endl;
}

template<typename T>
typename T::size_type GetLength(const T & c)
{
    if(c.empty())
    {
        return 0;
    }

    return c.size();
}

// typedef int(*funType)(int,int);



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


using FT = double(*)(string,string);


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

template<>
class myclasst<>
{
    public:
        myclasst()
        {
            printf("myclasst<>::myclasst()泛化版本执行了,this=%p\n",this);
        }
};

//模板模板参数
template<typename T,
         template<class> class Container    //这就是一个模板模板参数，血法比较固定，这里的名字叫做Container, 其实叫U也可以，因为模板模板参数一般是做容器用。
        >
class myclass
{
    public:
        T m_i;
        Container<T> myc;
};




int main()
{
    cout << funcadd<1,2>() << endl;
    MyArray<int,10> test;
    test.func();
    string testStr = "I Love China";
    string test2 = "";
    cout << GetLength(testStr) << endl;
    cout << GetLength(test2) << endl;
    testfunc(1,1,mf);

    tc tcobj;
    testfunc(3,4,tcobj);      //调用copy ctor

    testfunc(3,4,tc());      //调用ctor



    TC<char,char> tcchar;
    tcchar.functest();
    TC<int,int> tcint;
    tcint.functest();
    TC<double,int> tcdoubleint;
    tcdoubleint.functest();

    TCP<double,int,double> tcpdi;
    tcpdi.functest();
    TCP<int,int,double> tcpii;
    tcpii.functest();


    TCF<double> tcfd;
    tcfd.functest();
    TCF<const double> tcfcd;
    tcfcd.functest();

    //function 
    const char * p = "I Love China";
    int i = 12;
    tfunc(p,i);

    int k = 12;
    double db = 15.8f;
    tfunc(k,db);

    //multi template
    myfunct();
    myfunct(10,20);
    myfunct(10,25.6,"fdsf");


    cout << "-----" << endl;
    // myfunct2();     //错误语法，第一个参数必须要有
    myfunct2(10);
    myfunct2(10,25.6,"fadsf");

    cout << "...." << endl;
    OpenArgs(10,25.6,"fdasf");

    cout << "----" << endl;
    myclasst<int,float,double> myc;

    return 0;

}