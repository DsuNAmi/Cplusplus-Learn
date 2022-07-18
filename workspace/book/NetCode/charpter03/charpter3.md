# 地址族与数据序列

网卡来帮助把数据传输到对应的端口套接字。

TCP和UDP可以同时用一个端口号。

###### 表示IPV4地址的结构体

```c++
struct sockaddr_in
{
    sa_family_t		sin_family;		//地址族
    unint16_t 		sin_port;		//16位TCP/UDP端口号
    struct in_addr	sin_addr;		//32位IP地址
    char 			sin_zero[8];	//不使用
};

//该结构体中的另一个结构体
struct in_addr
{
    in_addr_t		s_addrl			//32位IPV4地址
};
```



### 结构体成员分析

###### sin_family

| 地址族   | 含义                       |
| -------- | -------------------------- |
| AF_INET  | IPV4网络协议中使用的地址族 |
| AF_INET6 | IPV6网络协议中使用的地址族 |

###### sin_port

保存16位端口号，它以网络字节序保存。

###### sin_addr

保存32位IP地址，也是用网络字节序保存。但是in_addr声明为uint32_t，所以其实只需要当整型看待。

###### sin_zero

无特殊含义。只是为了使结构体`sockaddr_in`和`sockaddr`结构体保持一致。

```C++
struct sockaddr {
	u_short	sa_family;
	char	sa_data[14];
};
```



## 网络字节序和地址变换

不同CPU中，4字节的整数型值1在内存空间的保存方式是不同的。比如：

```bash
00000000 00000000 000000000 00000001 //正序
00000001 00000000 000000000 00000000 //倒序
```



#### 字节序与网络字节序

CPU向内存保存数据的方式有两种，这意味着CPU解析数据的方式也有两种：

- 大端序（Big Endian）高位字节存放到低位地址
- 小端序（Little Endian）高位字节存放到高位地址

但是现在主流的CPU都是以小端序方式保存的字节序。

网络传输时统一使用的都是大端序。**先把数据数组转化成大端序格式再进行网络传输**。

**高位地位数据不是看数值本身，而是看位置。**

本来小端序表示的0x1234，大端序就是0x3412。`unsigned short`。



#### 字节序转换

- `htons`中的`h`代表的是主机字节序
- `htons`中的`n`代表的是网络字节序

另外，`s`指的是`short`，`l`指的是`long`。



## 网络地址的初始化与分配

INADDR_ANY可以自动获取运行服务器端的IP地址。

计算机中的IP地址数量和网卡数量相同。

```c++
in_addr_t inet_addr(const char * string);
//成功时返回32位大端序整型数值，失败时返回INADDR_NONE

int inet_aton(const char * string, struct in_addr * addr);
//成功时返回1，失败时返回0

char * inet_ntoa(struct in_addr add);
//成功时返回转换的字符串地址符，失败时返回-1
但是调用时要小心，返回指针说明字符串内存已经保存到内存空间，但是函数未向程序员要求分配内存，而是在内部申请了内存并保存字符串。也就是说，调用完函数之后，需要深复制该字符串到其他空间。否则，再次调用该函数可能会覆盖之前保存的字符串。
```

