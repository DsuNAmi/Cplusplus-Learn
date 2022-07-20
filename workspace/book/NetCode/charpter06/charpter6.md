# 基于UDP的服务器端/客户端

流控制是区分UDP和TCP的最重要标志。

TCP的速度无法超过UDP，但在收发某些类型的数据时有肯能接近UDP。例如：每次交换的数据量越大，TCP的传输速率就越接近UDP的传输速率。

原因主要有以下两点：

- 收发数据前后进行的连接设置及清除过程。
- 收发数据过程中为保证可靠而添加的流控制。

UDP客户端和服务端均只需要一个套接字。但是TCP对每个网卡都需要一个套接字。

因为TCP会保持连接状态，但是UDP不会，所以每次发送数据都需要添加目标地址信息。

```c++
//sendto
int sendto(SOCKET s, const char * buf, int len, int flags, const struct sockaddr* to, int tolen);
//成功时返回传输的字节数，失败时返回SOCKET_ERROR
//该函数同时也会自动分配IP地址和端口号，而TCP方法在connect函数。


//recvfrom
int recvfrom(SOCKET s, const char * buf, int len, int flags, struct sockaddr* from int * fromlen);
//成功时返回接收的字节数，失败时返回SOCKET_ERROR
```





因为UDP是存在数据边界（同步），也就是说输入函数的调用次数和输出函数的调用次数应该完全一致。



sendto函数传输的大致过程。

- 向UDP套接字注册目标IP和端口号
- 传输数据
- 删除UDP套接字中注册的目标地址信息。



但是如果我们能够保证在一段时间内与同一个主机进行交互，我们可以创建一个已连接套接字。

使用`connect`函数，之后与TCP连接方式相同。