# C++网络编程第一章

#### 构建服务器程序

- 调用 `socket`函数创建套接字
- 调用 `bind`函数分配IP地址和端口号
- 调用 `listen`函数转为可接收请求状态
- 调用 `accept`函数受理连接请求

#### `Winsock`的初始化

进行 `Winsock`编程时，首先必须调用 `WSAStartup`函数，设置程序中的用到的 `Winsock`版本，并初始化相应版本的库

```c++
#include <winsock2.h>

int WSAStartup(WORD wVersionRequested, LPWSADATA lpWSAData);
//成功时返回0，失败时返回非0的错误值代码
//wVersionRequested->程序要用的Winsock版本信息
//1pWSAData->WSDATA结构体变量的地址
```

- `wVersionRequested`

  应准备 `WORD(typedef usigned short WORD)`类型的套接字版本信息，并传递给该函数的第一个参数。如果版本是1.2，则1为主版本，2为副版本。

  借助 `MAKEWORD`宏函数能够轻松构建版本信息

  ```c++
  MAKEWORD(1,2); //主版本为1，副版本为2
  MAKEWORD(2,2); //主版本为2，副版本为2
  ```
- `lpWSAData`

  此参数需要传入 `WSADATA`型结构体变量地址。

一个初始化的例子：

```c++
#include <iostream>
#include <winsock2.h>

using namespace std;

int main()
{
    WSADATA wsaData;
    if(!WSAStartup(MAKEWORD(2,2),&wsaData))
    {
        cout << "WSAStartup error!" << endl;
    }   
    return 0;
}
```

#### `Winsock`的注销

```c++
#include <winsock2.h>

int WSACleanup(void);
//成功时返回0，失败是返回SOCKET_ERROR
```

调用该函数后，`Winsock`相关库将归还 `Windows`系统，无法再调用 `Winsock`相关函数。

## 基于Windows的套接字相关函数示例

- `socket`

  ```c++
  SOCKET socket(int af, int type, int protocol);
  //成功时返回套接字句柄，失败时返回INVAILD_SOCKET
  ```
- `bind`

  ```c++
  int bind(SOCKET s, const struct sockaddr * name, int namelen);
  //成功时返回0，失败时返回SOCKET_ERROR
  ```
- `listen`

  ```c++
  int listen(SOCKET s, int backlog);
  //成功时返回0，失败时返回SOCKET_ERROR
  ```
- `accept`

  ```c++
  SOCKET accept(SCOKET s, struct sockaddr * name, int *addrelen);
  //成功时返回套接字句柄，失败时返回INVALID_SOCKET
  ```
- `connect`

  ```c++
  int connect(SOCKET s, const struct sockaddr * name, int namelen);
  //成功时返回0，失败时返回SOCKET_ERROR
  ```
- `closesocket`

  ```c++
  int closesocket(SOCKET s);
  //成功时返回0，失败时返回SOCKET_ERROR
  ```

## 基于Windows的I/O函数

- `send`

  ```c++
  int send(SOCKET s, const char * buf ,int len, int flags);
  //成功时返回传输字节数，失败时返回SOCKET_ERROR
  ```

  > `s`->表示数据传输对象连接的套接字句柄值
  >
  > `buf`-保存待传输数据的缓冲地址值
  >
  > `len`->要传输的字节
  >
  > `flags`->传输数据时用到的多种选项信息
  >
- `recv`

  ```c++
  int recv(SOCKET s, const char * buf, int len, int flags);
  //成功时返回接受的字节数（收到EOF时为0），失败时返回SOCKET_ERROR
  ```

## 习题

###### 套接字的作用

所谓套接字(Socket)，就是对网络中不同主机上的应用进程之间进行双向通信的端点的抽象。一个套接字就是网络上进程通信的一端，提供了应用层进程利用网络协议交换数据的机制。从所处的地位来讲，套接字上联应用进程，下联网络协议栈，是应用程序通过网络协议进行通信的接口，是应用程序与网络协议栈进行交互的接口。

###### `listen`和 `accept`的区别

listen 将 套接字转换为可接受连接方式

accept 受理连接请求，并且在没有连接请求的情况下调用函数，不会返回。

二者存在逻辑上的先后关系

###### linux 对套接字进行数据IO可以直接使用文件函数，而Windows不可以，为什么

因为Linux下面万物皆文件，而Windows则是把两者分开。

###### 为什么要给套接字分配地址？使用哪个函数

因为要区分不同主机的套接字。使用 `bind()`。
