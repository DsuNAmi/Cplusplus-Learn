# 引用与指针

## 总论

| 引用         | 指针         |
| ------------ | ------------ |
| 必须初始化   | 可以不初始化 |
| 不能为空     | 可以为空     |
| 不能更换目标 | 可以更换目标 |



我们在定义一个引用的时候必须为其指定一个初始值，但是指针却不需要。

```c++
int & r; //error 没有初始化
int * p; //野指针
```



用于引用不能为空，所以我们在使用引用的时候不需要测试其合法性，而在使用只指针的时候首先判断是否为空指针，否则可能会引起程序崩溃。

```c++
void testP(int * p)
{
    if(!p) *p = 3;
    return;
}

void testR(int & r)
{
    r = 3;
    return;
}
```



指针可以随时改变指向，但是引用只能指向初始化时指向的对象，无法改变。

```c++
int a = 1;
int b = 2;

int & r= a; //初始化
int *p = &a; 

p = &b; //指针指向了变量b
r = b; //引用依然指向a，但是a的值变成了b
```



## 引用

###### 左值引用

常规引用，一般表示对象的身份。

###### 右值引用

右值引用就是必须绑定到右值（一个临时对象，将要销毁的对象）的引用，一般表示对象的值。

右值引用可实现转移语义和精确传递：

- 消除两个对象交互时不必要的对象拷贝，节省运算存储资源，提高效率。
- 能够更简洁明了的定义泛型函数

