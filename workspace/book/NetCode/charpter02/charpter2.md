# 套接字类型与协议设置

## 协议族（Protocol Family）

| name     | PF               |
| -------- | ---------------- |
| PF_INET  | IPv4互联网协议族 |
| PF_INET6 | IPv6互联网协议族 |
| PF_IPX   | IPX Novell协议族 |

在函数`socket`中，套接字实际采用的最终协议信息时通过`socket`函数的第三个参数传递的。在指定的协议族范围内通过第一个参数决定第三个参数。



## 套接字类型（Type）

套接字类型指的套接字的数据传输方式，通过`socket`函数的第二个参数传递，只有这样才能决定创建的套接字的数据传输方式。

因为决定了协议族也不能决定是哪一种传输方式。

###### 面向连接的套接字（SOCK_STREAM）

特点：

- 传输过程中数据不会消失。
- 按序传输数据。
- 传输的数据不存在数据边界（boundary）。100个糖果是分批传输的，但接收者凑齐100个才装袋。**也就是不同步的**。
- 套接字连接必须一一对应。

手法数据的套接字内部有缓冲。收到数据之后不一定会马上接收，只要不超过容量，可能是一次性全部接收，也可能是分批接收。

###### 面向消息的套接字（SOCK_DGRAM）

特点：

- 强调快速传输而非传输顺序。
- 传输的数据可能丢失也可能损毁。
- 传输的数据有数据边界（同步的）。
- 限制每次传输的大小。

## 协议的最终选择

传递前面两个参数即可创建所需要的套接字。所以大部分情况下可以向第三个参数传递0，除非：
**同一个协议族中存在多个数据传输方式相同的协议**。

但是一般来说，满足PF_INET和上述两种套接字类型的只有TCP和UDP。



# 习题

###### 什么是协议？在收发数据中有什么意义？

协议就是一种约定好的规则。

###### 何种类型的数据类型的套接字不存在数据边界？这类套接字接收数据时需要注意什么？

SOCK_STREAM不存在数据边界。因此输入输出函数响应次数不具有意义。重要的不是函数的响应次数，而是数据的收发量。所以，必须将传输数据的量和接收数据的量制作成编码，保证发送数据的量和接收数据的量是一致的。













