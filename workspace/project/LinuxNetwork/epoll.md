# EPOLL

这里介绍介个函数

###### epoll_create函数

```c++
int epoll_create(int size);
//创建一个epoll对象，返回一个对象（文件）描述符来表示该epoll对象，后续要通过该描述符进行数据的收发

该对象最终是要被close的，因为这就是类似套接字的一个句柄;
size的值不管怎么解释，这个值一定要大于0;


struct eventpoll * ep = (struct eventpoll *)calloc(1, sizeof(struct eventpoll));
//生成一个eventpoll对象。
eventpoll中我们只关心rbr和rblist;
#rbr,可以将该成员理解为一颗红黑树根节点;
#rdlist,可以将成员理解为一个双向链表的表头指针;
```

###### epoll_ctl函数

```c++
int epoll_ctl (int efpd, int op, int sockid,  struct epoll_event * event);
//把一个socket及该socket相关的事件添加到epoll对象描述符中，以通过该epoll对象来监视该socket上数据的来往情况，当有数据来往时，系统会通知程序
参数efpd，从epoll_create返回的对象描述符;
参数op，一个操作类型，可以理解为一个数字，一般是1，2，3
    1 添加sockid上关联的事件，对应一个宏定义EPOLL_CTL_ADD
    2 删除sockid上关联的事件，对应一个宏定义EPOLL_CTL_DEL
    3 修改sockid上关联的事件，对应一个宏定义EPOLL_CTL_MOD
    添加就是把socket绑定在红黑树上，让epoll对象去监听这个对象，有消息就通知。
    这里要注意删除只是把socket从红黑树上删除，而不会关闭这个socket连接，删除了就不会被通知到这个epoll对象。
参数sockid，一个TCP连接。sockid就是红黑树的key值;
参数event，传递对应的信息;
```

###### epoll_wait函数

```c++
int epoll_wait(int epfd, struct epoll_event * events, int maxevents, int timeout);
//阻塞一小段时间并等待事件发生，返回事件集合，即获取内核的事件通知。
//这里就要是遍历epoll对象中的双向链表，数据都在里面记录
参数epfd，从epoll_create返回的epoll对象;
参数events，一个数组，长度为maxevents，表示此次调用可以最多收集的读写事件。;
参数timeout，阻塞等待的时长。;

这就是epoll高效的原因，是因为不是从所有的socket中遍历是否有事件，而是从有事件的中寻找socket;
```

EPOLL有两种工作模式

- LT，Level Trigged，即水平触发，低速模式

  默认采用LT。

  发生一个事件，如果程序不处理，那么该事件就一直触发。
- ET，Edge Trigged，即边缘触发，高速模式

  这种触发方式只对非阻塞socket有用。发生一个事件，内核只会通知程序1次，不会再通知。如果没有处理就没有处理了，不会一直喊着让处理。

###### 一道面试题

**题目**：使用Linux Epoll模型，水平触发模式。当socket可写时，会不停地触发socket可写的事件。

**答案**：（1）需要向socket写数据的时候才把socket可写事件通知加入epoll的红黑树节点，等待可写事件。当程序收到来自系统的可写事件通知后，调用write或read发送数据。所有数据都发送完毕后，把socket可写事件通知从epoll的红黑树节点中**移除（移除的是可写事件通知，不是红黑树节点）**。这种方式的缺点是，即使发送很少的数据，也要把socket可写事件通知加入epoll的红黑树节点，写完后再把可写事件通知从epoll的红黑树节点中移除，有一定的操作代价。

    （2）开始不把socket可写事件通知加入epoll的红黑树节点，需要发数据时，直接调用write或send发送，如果write或者send返回EAGAIN（缓冲区满了，需要等待可写事件才能继续往发送缓冲区写数据），再把socket的写事件通知加入epoll的红黑树节点。这就变成了在epoll的驱动下发送数据，全部数据发送完毕后，再把可写事件通知从epoll的红黑树节点中移除。这种方式的优点是，数据不多时可以避免epoll的红黑树节点中针对写事件通知的增删，提高了程序执行效率。

### TCP粘包

###### 客户端粘包

客户端采用了**Nagle算法**，客户端3次调用write函数发送数据，Nagle算法可能会把这3次调用打成一个数据包发送给服务器。这就是客户端粘包。但是可以关闭Nagle算法，这样就是3个包，但是每个包都打包包头，这样效率会低一些。

###### 服务器粘包

服务器端不可能随时都在recv数据，可能recv一次之后要做一些其他操作，需要时间，假设这期间客户端发送的3个数据包到了服务器端，保存在服务器端的接收缓冲区中。等待服务器完成其他操作，过来之后，服务器还是会把3个包的数据全部一次收到。这就是服务器粘包。

服务器在收包时，因为网络拥堵情况等因素，收到多少数据都有可能，可能数据包小，一次性就收完了，可能数据包大，要**几次才能收完一个完整的包（缺包）**。

###### 问题解决

想要正确解决粘包问题，需要程序员给收发的数据包定义一个统一的格式，服务器端及客户端都按照该格式来收发数据。

这个格式就是**包头+包体**的格式。收发的任何一个数据包，都要遵循这种包头+包体的格式。其中包头是固定长度的。包头其实是一个结构，在包头中要有一个成员，用于记录整个包的长度（包头+包体）。因为包头长度固定，且能够从包头中获取整个包的长度，用整个包的长度减去包头的长度，就得到了包体的长度。

# 线程池

就是提前创建一批线程，并创建一个类来统一管理和调度这批线程（这一批线程构成一个池子，形象称为线程池。）收到一个任务（一个消息），就从这批线程中找一个空闲的去做这个任务（处理这个消息）。任务完成之后，线程中有个循环语句，可以使其循环回来等待新任务，就好像这个线程可以回收再利用一样。

# 心跳包

心跳包就是一个程序员自定义格式的数据包，当然，也要符合本项目规定的包头+包体的格式要求。

心跳包一般都是每隔10s~60s由客户端主动发送给服务器，服务器收到后一般也会 给客户端回应一个心跳包。

如果客户断不向服务器发送心跳包，那么服务器会怎么样？会有一个时间限制，如果时间限制过了，收不到心跳包了，那么就会关闭与该客户端之间的连接。

心跳包主要是用来判断对方还是否在线。