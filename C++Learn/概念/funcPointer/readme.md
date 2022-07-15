# 函数指针

###### 函数的地址

只要使用函数名（后面不跟参数）即可。

```c++
process(think); //process 内部调用think()；
thought(think()); //接收think()返回的值
```



###### 声明函数指针

声明指向函数的指针时，也必须指定指向函数的类型。这意味着声明应指定函数的返回类型以及函数的特征标（函数参数）。

```c++
double pam(int); //函数原型
double (*pf)(int); //pf为函数指针
```



通常，要声明特定类型的函数的指针，可以首先编写这种函数的原型，然后用`(*pf)`替换函数名，这样`pf`就是函数的指针。



```c++
double pam(int);
double (*pf)(int);
pf = pam; //赋予指针值

void estimate(int line, double (*pf)(int));

estimate(50,pam); //传递指针
```



`pam()`的特征标和返回类型必须与`pf`相同。如果不相同，编译器将拒绝这种赋值。



###### 使用指针来调用函数

``` c++
double x = pam(4);
double y = (*pf)(5);
//实际上，C++也允许使用函数名那样使用pf
double y = pf(5);
```



```c++
#include <iostream>

double betsy(int);
double pam(int);


void estimate(int lines, double (*pf)(int));


int main()
{
    using namespace std;
    int code;
    cout << "How many lines of code do you need? ";
    cin >> code;
    cout << "Here's Betsy's estimate: \n";
    estimate(code,betsy);
    cout << "Here's Pam's estimate: \n";
    estimate(code,pam);
    return 0;
}

double betsy(int lns) {return 0.05 * lns;}

double pam(int lns) {return 0.03 * lns + 0.0004 * lns * lns;}

void estimate(int lines, double (*pf)(int))
{
    using namespace std;
    cout << lines << " lines will take ";
    cout << (*pf)(lines) << " hour(s)\n";    
}
```



## 深入探讨函数指针

函数指针的表示可能非常恐怖。首先，下面是一些函数的原型，它们的特征标和返回类型相同。

```c++
const double * f1(const double ar[], int n);
const double * f2(const double [], int);
const double * f3(cosnt double *, int);
```



接下来，假设要声明一个指针，它可指向这三个函数之一

```c++
const double * (*p1)(const double *, int);//声明
const double * (*p1)(const double *, int) = f1; //声明时进行初始化
auto p2 = f2; //自动推断类型
```

使用

```c++
cout << (*p1)(av,3) << ": " << *(*p1)(av,3) << endl; //前面是地址，后面是值
cout << p2(av,3) << ": "  << *p2(av,3) << endl;
```



为了能够循环调用这三个函数，我们可以声明一个函数指针数组

```c++
const double * (*pa[3])(const double *, int) = {f1,f2,f3};
```

调用

```c++
const double * px = pa[0](av,3);
const double * py = (*pa[1])(av,3);
```



### 使用typedef进行简化

```c++
typedef const double * (*p_fun)(const double *, int);
p_fun p1 = f1;
p_fun pa[3] = {f1,f2,f3};
p_fun (*pd) = &pa;
```

