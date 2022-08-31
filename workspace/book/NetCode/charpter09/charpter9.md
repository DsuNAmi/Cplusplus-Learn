# 套接字的多种可选项

### 套接字可选项和IO缓冲大小

###### 套接字多种可选项

| 协议层     | 选项名            | 读取 | 设置 |
| ---------- | ----------------- | ---- | ---- |
| SOL_SOCKET | SO_SNDBUF         | O    | O    |
| SOL_SOCKET | SO_RCVBUF         | O    | O    |
| SOL_SOCKET | SO_REUSEADDR      | O    | O    |
| SOL_SOCKET | SO_KEEPALIVE      | O    | O    |
| SOL_SOCKET | SO_BROADCAST      | O    | O    |
| SOL_SOCKET | SO_DONETROUTE     | O    | O    |
| SOL_SOCKET | SO_OOBINLINE      | O    | O    |
| SOL_SOCKET | SO_ERROR          | O    | X    |
| SOL_SOCKET | SO_TYPE           | O    | X    |
| IPPROTO_TP | IP_TOS            | O    | O    |
| IPPROTO_IP | IP_TTL            | O    | O    |
| IPROTO_IP  | IP_MULTICAST_TTL  | O    | O    |
| IPROTO_IP  | IP_MULTICAST_LOOP | O    | O    |
| IPROTO_IP  | IP_MULTICAST_IF   | O    | O    |
| IPROTO_TCP | TCP_KEEPALIVE     | O    | O    |
| IPROTO_TCP | TCP_NODELAY       | O    | O    |
| IPROTO_TCP | TCP_MAXSEG        | O    | O    |



套接字选项是分层的。IPROTO_IP层可选项是IP协议相关事项，IPROTO_TCP层可选项是TCP协议相关事项，SOL_SOCKET是套接字相关事项。



###### 两个函数

```c++
int getsocketopt(SOCKET sock, int level, int optname, char * optval, int * optlen);
//成功时返回0，失败时返回SOCKET_ERROR
sock, 套接字句柄;
level, 要查看的可选项协议层;
optname, 要查看的选项名;
optval, 保存查看结果的缓冲地址;
optlen, optval的缓冲大小。调用结束后，该变量中保存通过第四个参数返回可选项字节数;
```



```c++
int setsocketopt(SOCKET sock, int level, int optname, const char * optval, int optlen);
//成功时返回0， 失败时返回SOCKET_ERROR

```



### 发生地址分配错误

来看回声服务器端的一个例子：

```c++
#include <iostream>
#include <winsock2.h>

using namespace std;

int main()
{
    WSADATA wsadata;
    
    WSAStartup(MAKEWORD(2,2),&wsadata);

    SOCKET serverSock;
    SOCKET clientSock;
    SOCKADDR_IN serverAddr;
    SOCKADDR_IN clientAddr;
    int optlen;
    int clientAddrSize;
    int option;
    int strlen;
    char message[30];
    char * port = "8080";

    serverSock = socket(PF_INET,SOCK_STREAM,0);
    
    optlen = sizeof(option);
    option = 1;
    setsockopt(serverSock,SOL_SOCKET,SO_REUSEADDR,(char*)&option,optlen);

    memset(&serverAddr,0,sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(atoi(port));

    bind(serverSock,(SOCKADDR*)&serverAddr,sizeof(serverAddr));

    listen(serverSock,5);

    clientAddrSize = sizeof(clientAddr);
    clientSock = accept(serverSock,(SOCKADDR*)&clientAddr,&clientAddrSize);


    while(strlen = recv(clientSock,message,sizeof(message),0))
    {
        send(clientSock,message,strlen,0);
        send(1,message,strlen,0);//
    }

    closesocket(clientSock);
    closesocket(serverSock);


    return 0;
}
```



其中有一个区别，就是如果是客户端先断开连接，那么回向服务器发送FIN信息。但是如果是强制关闭服务器，那么是服务器会向客户端发送FIN消息。如果是这种情况，那么再次启动服务器时，就会出现bind error的问题。但是过几分钟之后再启动就问题。



###### Time-wait状态

套接字经过四次握手过程后并非立即消除，而是要经过一段时间的Time-wait状态。当然，只有先断开连接的（先发送FIN信息）主机才经过time-wait状态。因此，如果服务器先断开连接，那么会处于端口被占用的time-wait状态。

如果服务器端最后一个FIN消息没有发送到客户端，这时候如果没有time-wait状态，那么客户端就永远都收不到来自服务器的最后一个ACK了。如果有TIME-WAIT，会等待客户端重发FIN消息后再发送ACK。

但是每次接收到重传后，time-wait计时器会重新启动。

而客户端每次启动都是自动分配端口号，所以不需要在意客户端的time-wait状态，每次启动的端口号都不相同。

而SO_REUSEADDR的作用就是可将time-wait状态下的套接字端口号重新分配给新的套接字。默认值是0，意味着time-wait时无法分配，改为1即可分配。



###### Nagle算法

为了防止因为数据包过多而发生网络负载。

Nagle算法：

**只有收到前一数据的ACK消息时，Nagle算法才会发送吓一条消息**

TCP套接字默认使用Nagle算法，因此最大限度进行缓冲，直到接收到ACK。

而：

**Nagle算法使用与否与在网络流量上差别不大， 使用Nagle算法传输的更慢**

禁用nagle算法：

```c++
int optval = 1;
setsocketopt(sock,IPROTO_TCP,TCP_NODELAY,(char*)&optval,sizeof(optval));
```

