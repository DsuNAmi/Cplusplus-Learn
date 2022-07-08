# 位域

Bit Field 为一种数据结构，可以把数据以位形式紧凑的存储，并允许程序员对此结构的位进行操作。这种数据结构的一个好处是它可以使数据单元节省存储空间，当程序需要成千上万个数据单元时，这种方法就显得尤为重要。

第二个好处使位域可以很方便的访问一个整数值的部分内容从而可以简化程序源代码。而这种数据结构的缺点在于，位域实现依赖于具体的机器和系统，在不同的平台可能有不同的结果，这导致了位域在本质是不可移植的。

- 位域在内存中的布局是与机器有关的。
- 位域的类型必须是整型或者是枚举型，带符号型中的位域的行为因具体实现而定。
- 取地址运算符`&`不能作用于位域，任何指针都无法指向类的位域。

## 使用

位域通常使用结构体声明，该结声明为每个位域成员设置名称，并决定其宽度：

```c++
struct bitFieldName
{
   type memberName : width;
}
```

| Elements     | Description                                                  |
| ------------ | ------------------------------------------------------------ |
| bitFieldName | 位域结构名                                                   |
| type         | 位域成员的类型，必须为`int`，`signed int`或者是`unsigned int`类型 |
| memberName   | 位域成员名                                                   |
| width        | 规定成员所占的位数                                           |

声明一个位域：

```c++
struct _PRCODE
{
    unsigned int code1 : 2;
    unsigned int code2 : 2;
    unsigned int code3 : 8;
};

struct _PRCODE prcode;
```

该定义使`precode`包含两个`2Bits`位域和一个`8Bits`位域，我们可以使用结构体的成员运算符对其赋值

```c++
prcode.code1 = 0;
prcode.code2 = 3;
prcode.code3 = 102;
```

赋值时要注意值的大小不能超过位域成员的容量，例如：`prcode.code3`为`8Bits`的位域成员，其容量为$2^8= 256$，即赋值范围应该为[0,255]。

## 位域的大小和对齐

###### 位域的大小

```c++
struct box
{
    unsigned int a : 1;
    unsigned int   : 3;
    unsigned int b : 4;
}
```

该位域结构体中间有一个未命名的位域，占据`3Bits`，仅起填充作用。填充使得该结构总共使用了`8Bits`。但C语言使用`unsigned int`作为位域的基本单位，即使一个结构体的唯一成员为`1Bits`的位域，该结构体的大小也和一个`unsigned int`大小相同。

###### 位域的对齐

一个位域成员不允许跨越两个`unsigned int`的边界，如果成员声明的总位数超过了一个`unsigned int`的大小，那么编译器会自动移位位域成员，使其按照`unsigned int`的边界对齐，例如：

```c++
struct stuff
{
    unsigned int field1 : 30;
    unsigned int field2 : 4;
    unsigned int field3 : 3;
}
```

`field1 + field2 = 34Bits` ，超出了`32Bits`。编译器会将`field2`移位至下一个`unsigned int `单元存放，`stuff.field1 stuff.field2`之间会留下一个`2Bits`的空隙，所以该结构的大小为`2 * 32 = 64Bits`。

这个空洞可以用之前提到的未命名的位域成员填充，我们也可以使用一个宽度为0的未命名成员位域令下一位位域成员与下一个整数对齐

```c++
struct stuff
{
    unsigned int field1 : 30;
    unsigned int        : 2;
    unsigned int field2 : 4;
    unsigned int        : 0;
    unsigned int field3 : 3;
}
```

这里`stuff.field3`存储在下一个`unsigned int`中，该结构体大小为`3*32 = 96Bits`。

##  位域的初始化和位的重映射

###### 初始化

位域的初始化与普通结构体的方法相同：

```c++
struct stuff s1 = {20,8,6};
```

或者直接给位域成员赋值

```c++
struct stuff s1;
s1.field1 = 20;
s1.field2 = 8;
s1.field3 = 4;
```

###### 位域的重映射

声明一个大小为`32Bits`的位域。

```c++
struct box
{
    unsigned int ready : 2;
    unsigned int error : 2;
    unsigned int command : 4;
    unsigned int sectorNo : 24;
}b1;
```

###### 利用重映射将位域归零

```c++
int * p = (int * )&b1; //将"位域结构体的地址"映射至"整型（int*）"的地址
*p = 0;//将各成员归零。
```



