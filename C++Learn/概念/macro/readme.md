# 宏

## 宏中包含的特殊符号

`#,##,\`

###### 字符串化操作符

在一个宏的参数前面使用一个`#`，预处理器会把这个参数转换成一个字符数组。`#`就是**字符串化**的意思，出现在宏定义中的`#`是把跟在后面的参数转换成一个字符串。



注意：其只能用于传入参数的宏定义中，且必须置于宏定义体中的参数名前。



```c++
#include <iostream>
#include <string>

using namespace std;

#define exp(s) printf("test s is: %s \n", s)
#define exp1(s) printf("test s is: %s \n", #s)
#define exp2(s) #s

int main()
{
    exp("hello");
    exp1(hello);

    string str = exp2(    bac  );
    cout << str << " " << str.size() << endl;

    string str1 = exp2(  asf  fds);
    cout << str1 << " " << str.size() << endl;

    return 0;
}
```



- 忽略传入参数名前面和后面的空格
- 字符串之间有空格是，会自动连接每个子字符串，用空格分隔。



###### 符号操作连接符

`##`是一种分隔连接方式，它的作用是先分隔，然后进行强制连接。将定义的多个形参转换成一个实际参数名。

- 当用`##`连接时，`##`前面的空格可有可无
- 连接后的实际参数名，必须为实际存在的参数或是编译器已知的宏定义。
- 如果`##`后的参数本身也是一个宏，`##`会阻止这个宏的展开。

```c++
#include <iostream>

using namespace std;

#define expA(s) printf("prefix added: %s \n", gc_##s) //gc_s 必须存在
#define expB(s) printf("prefix added: %s \n", gc_  ##  s) //gc_s必须存在
#define gc_hello1 "I am gc_hello1"

int main()
{
    const char * gc_hello = "I am gc_hello";
    expA(hello);
    expB(hello1);
    return 0;
}
```



###### 续行操作符

当定义的宏不能用一行表达完整时，可以用`\`表示下一行继续此宏的定义。

**\前留空格**

```c++
#include <iostream>

using namespace std;


#define MAX(a,b) ((a) > (b) ? (a) \
    : (b))


int main()
{
    int maxVal = MAX(3,6);
    cout << maxVal << endl;
    return 0;
}
```



###### 避免曲解语义

```c++
#define fun() f1(); f2();
if(a > 0)
    	fun()
```

被宏展开之后

```c++
if(a > 0)
    f1();
	f2();
```

本意是执行`f1,f2`，而实际是`f2`都会执行，所以就错误了。

为了解决这种问题，在写代码的时候，通常可以采用`{}`块。



```c++
#define fun() {f1(); f2();}

if(a > 0)
    fun();
//宏展开
if(a > 0)
{
    f1();
    f2();
}；
```



###### 避免使用`goto`控制流。

在一些函数中，我们可能需要在`return`语句之前做一些清理工作，比如释放在函数开始处由`malloc`申请的内存空间，使用`goto`总是一种简单的方法：

```c++
int f()
{
    int * p = (int *)malloc(sizeof(int));
    *p = 10;
    cout << *p << endl;

#ifndef DEBUG
    int error = 1;
#endif
    if(error)
        goto END;
    //dosomething
END:
    cout << "free" << endl;
    free(p);
    return 0;
}
```



但由于`goto`不符合软件工程的结构化，而且可能使得代码难懂，所以很多都不倡导使用，这个时候我们可以使用`do{...}while(0)`来做同样的事情：

```c++
int ff()
{
    int * p = (int *)malloc(sizeof(int));
    *p = 10;
    cout << *p << endl;

    do{
        #ifndef DEBUG
            int error = 1;
        #endif
            if(error) break;
        //dosomething
    }while(0);
    cout << "free" << endl;
    free(p);
    return 0;
}
```



###### 避免由宏引起的警告

内核中由于不同架构的限制，很多时候会用到空宏。在编译的时候，这些宏会给出`warnning`，为了避免这样的`warnning`，我们可以使用`do{...}while(0)`。

```c++
#define EMPTYMICRO do{}while(0)
```



###### 定义单一的函数块来完成复杂的操作

如果你有一个复杂的函数，变量很多，而且你不要增加新的函数，可以使用`do{...}while(0)`，将你的代码写在里面，里面可以定义变量而不用考虑变量名会同名函数之前或者之后的重。这种情况应该是指一个变量多处使用，我们可以在每个`do-while`中缩小作用域：

```c++
int fc()
{
    int k1 = 10;
    cout << k1 << endl;
    do{
        int k1 = 100;
        cout << k1 << endl;
    }while(0);
    cout << k1 << endl;
}
```



