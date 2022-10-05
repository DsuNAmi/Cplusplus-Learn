# 第十二章 I/O复用

### 基于I/0复用的服务器端

#### 多进程服务器端的缺点和解决方法

为了构建并发服务器，只要有客户端连接就会创建新进程。但是进程多了也未必是一件好事，所以I/O复用就能够在不创建进程的同时向多个客户端提供服务。

#### 理解复用

复用：为了提高物理设备的效率，用最少的物理要素传递最多数据时使用的技术。

### 理解select函数并实现服务器端

###### `select`函数的功能和调用顺序

步骤一：

- 设置文件描述符
- 指定监视范围
- 设置超时

步骤二：

- 调用`select`函数

步骤三：

- 查看调用结果



```c++
#include <winsock2.h>

int select(int nfds, fd_set * readfds, fd_set * writefds, fd_set * excepfds, const struct timeval * timeout);
//成功时返回大于0， 失败是返回-1
//nfds， 监视对象文件描述符对象
//readfds, 将所有 是否存在待读取数据 的文件描述符注册到fd_set型变量，并传递其地址值
//writefds， 将所有 是否可传输无阻塞数据 的文件描述符注册到fd_set型变量，并传递其地址值
//exceptfds, 将所有 是否发生异常 的文件描述符注册到fd_set型变量，并传递其地址值。
//timeout, 调用select函数后，为防止陷入无限阻塞的状态，传递超时信息。

typedef struct timeval
{
    long tv_sec;
    long tv_usec;
} TIMEVAL;


typedef struct fd_set
{
    u_int fd_count;
    SOCKET fd_array[FD_SETSIZE];
} fd_set;
```

