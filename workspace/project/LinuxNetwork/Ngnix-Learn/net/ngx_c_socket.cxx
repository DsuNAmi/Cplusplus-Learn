#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>
// #include <sys/socket.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
// #include <pthread.h>


#include "ngx_c_conf.h"
#include "ngx_macro.h"
#include "ngx_global.h"
#include "ngx_func.h"
#include "ngx_c_socket.h"
#include "ngx_c_memory.h"
#include "ngx_c_lockmutex.h"

CSocket::CSocket()
{
    // mListenPortCount = 1;       //监听一个端口
    // mWorkerConnections = 1;

    // //epoll
    // mEpollHandle = -1;
    // mPConnections = nullptr;
    // mPFreeConnections = nullptr;



    // miRecvMsgQueueCount = 0;

    // pthread_mutex_init(&mRecvMessageQueueMutex, nullptr);



    mWorkerConnections = 1;     //epoll连接最大项数
    mListenPortCount = 1;       //监听一个端口
    mRecyConnectionWaitTime = 60;       //等待1分钟后才回收连接池

    mEpollHandle = -1;

    miLenPkgHeader = sizeof(COMM_PKG_HEADER);
    miLenMsgHeader = sizeof(STRUC_MSG_HEADER);
    
    miSendMsgQueueCount = 0;
    mTotolRecyConnectionN = 0;
    mCurSize = 0;
    mTimerValue = 0;

    return;
}

// bool CSocket::Initialize()
// {
//     ReadConf();

// }

CSocket::~CSocket()
{
    //释放之前的指针
    std::vector<lpngxListeningT>::iterator pos;
    for(pos = mListenSocketList.begin(); pos != mListenSocketList.end(); ++pos)
    {
        delete (*pos);
    }
    mListenSocketList.clear();


    //epoll释放
    // if(mPConnections != nullptr)
    // {
    //     delete [] mPConnections;
    // }

    // ClearMsgRecvQueue();


    // pthread_mutex_destroy(&mRecvMessageQueueMutex);

    return;
}

// void CSocket::ClearMsgRecvQueue()
// {
//     char * sTempMemPoint;
//     CMemory * pMemory = CMemory::GetInstance();

//     while(!mMsgRecvQueue.empty())
//     {
//         sTempMemPoint = mMsgRecvQueue.front();
//         mMsgRecvQueue.pop_front();
//         pMemory->FreeMemory(sTempMemPoint);
//     }
// }

//初始化函数【fork()】子进程之前干这个事
//成功返回true,失败返回false
bool CSocket::Initialize()
{
    ReadConf();
    if(NgxOpenListeningSockets() == false)
    {
        return false;
    }
    return true;
}

//子进程才需奥执行的初始化函数
bool CSocket::InitializeSubProc()
{
    //发消息互斥量初始化
    if(pthread_mutex_init(&mSendMessageQueueMutex,nullptr) != 0)
    {
        NgxLogStderr(0,"CSocket::INitialize()' s pthread_mutex_init(sendmsg) failed");
        return false;
    }
    //连接相关的互斥量初始化
    if(pthread_mutex_init(&mConnctionMutex,nullptr) != 0)
    {
        NgxLogStderr(0,"CSocket::Initialize()'s pthread_mutex_init(connetion) failed");
        return false;
    }
    //连接池回收的信号量初始化
    if(pthread_mutex_init(&mRecyConnQueueMutex,nullptr) != 0)
    {
        NgxLogStderr(0,"CSocket::Initialize()'s pthread_mutex_init(recy) failed");
        return false;
    }

    //处理和时间有关的互斥量
    if(pthread_mutex_init(&mTimeQueueMutex,nullptr) != 0)
    {
        NgxLogStderr(0,"CSocket::Initialize()'s pthread_mutex_init(time) failed");
        return false;
    }

    //初始化发消息信号量,用于进程和线程之间的同步,
    //第二个参数=0,表示信号量在线程之间共享,如果不是0,那么在进程之间共享
    //第三个参数=0,表示信号量的初始值为0,为0时,调用sem_wait()就会在那里卡着
    if(sem_init(&mSemEventSendQueue,0,0) == -1)
    {
        NgxLogStderr(0,"CSocket::Initialize()_subproc's sem_init(semEvent)");
        return false;
    }

    //创建线程，一个是用来给客户端发送年数据的，一个是用来做回收池处理的
    int err;
    ThreadItem * pSendQueue;
    mThreadVector.push_back(pSendQueue = new ThreadItem(this));
    err = pthread_create(&pSendQueue->_Handle,nullptr,ServerSendQueueThread,pSendQueue);
    if(0 != err)
    {
        return false;
    }

    ThreadItem * pRecyConn;
    mThreadVector.push_back(pRecyConn = new ThreadItem(this));
    err = pthread_create(&pRecyConn->_Handle, nullptr, ServerRecyConnectionThread,pRecyConn);
    if(0 != err)
    {
        return false;
    }


    if(mifKickTimeCount == 1)
    {
        ThreadItem * pTimePoint;
        mThreadVector.push_back(pTimePoint = new ThreadItem(this));
        err = pthread_create(&pTimePoint->_Handle,nullptr,ServerTimeQueueMonitorThread,pTimePoint);
        if(0 != err)
        {
            NgxLogStderr(0,"CSocket::Initialize_subproc()'s pthread_create(time) failed");
            return false;
        }
    }

    return true;
}


//关闭退出函数
void CSocket::ShutDownSubProc()
{


    //(1)把干活的线程停止掉,注意，系统应该尝试通过设置gStopEvent = 1来让整个项目停止
    if(sem_post(&mSemEventSendQueue) == -1)
    {
        NgxLogStderr(0,"CSocket::ShutdownSubProc()'s sem_post() failed");
    }

    std::vector<ThreadItem*>::iterator iter;
    for(iter = mThreadVector.begin(); iter != mThreadVector.end(); ++iter)
    {
        pthread_join((*iter)->_Handle,nullptr);     //等待所有的线程停止
    }

    //(2)释放一下new出来的ThreadItem
    for(iter = mThreadVector.begin(); iter != mThreadVector.end(); ++iter)
    {
        if(*iter)
        {
            delete *iter;
        }
    }
    mThreadVector.clear();

    //(3)队列相关
    ClearMsgSendQueue();
    ClearConnection();
    ClearAllFromTimerQueue();

    //(4)多线程相关
    pthread_mutex_destroy(&mConnctionMutex);
    pthread_mutex_destroy(&mSendMessageQueueMutex);
    pthread_mutex_destroy(&mRecyConnQueueMutex);
    pthread_mutex_destroy(&mTimeQueueMutex);
    sem_destroy(&mSemEventSendQueue);
}


//清理TCP发送消息队列
void CSocket::ClearMsgSendQueue()
{
    char * sTempMemPoint;
    CMemory * pMemory = CMemory::GetInstance();

    while(!mMsgSendQueue.empty())
    {
        sTempMemPoint = mMsgSendQueue.front();
        mMsgSendQueue.pop_front();
        pMemory->FreeMemory(sTempMemPoint);
    }
}


void CSocket::ReadConf()
{
    CConfig * pConfig = CConfig::GetInstance();
    mWorkerConnections = pConfig->GetIntDefault("worker_connections",mWorkerConnections);
    mListenPortCount = pConfig->GetIntDefault("ListenPortCount",mListenPortCount);
    mRecyConnectionWaitTime = pConfig->GetIntDefault("Sock_RecyConnectionWaitTime",mRecyConnectionWaitTime);


    mifKickTimeCount = pConfig->GetIntDefault("Sock_WaitTimeEnable",0);
    //多少秒检测一次心跳超时
    miWaitTime = pConfig->GetIntDefault("Sock_MaxWaitTime",miWaitTime);
    miWaitTime = (miWaitTime > 5) ? miWaitTime : 5;
    miFTimeOutKick = pConfig->GetIntDefault("Sock_TimeOutKick",0);

    mFloodAttackEnable = pConfig->GetIntDefault("Sock_FloodAttackKickEnable",0);
    mFloodTimeInterval = pConfig->GetIntDefault("Sock_FloodTimeInterval",100);
    mFloodKickCount = pConfig->GetIntDefault("Sock_FloodKickCounter",10);
    



    // mWorkerConnections = pConfig->GetIntDefault("worker_connections",mWorkerConnections);
    // mListenPortCount = pConfig->GetIntDefault("ListenPortCount",mListenPortCount);

    return;
}



//监听端口【支持多个端口】，
//在创建worker进程之前就要执行这个函数
bool CSocket::NgxOpenListeningSockets()
{
    CConfig * pConfig = CConfig::GetInstance();

    int         isock;//socket
    struct sockaddr_in servAddr;       //服务器的地址结构丢
    int         iport;              //端口
    char        strInfo[100];       //临时字符串


    //初始化相关
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);


    for(int i = 0; i < mListenPortCount; ++i)
    {
        isock = socket(AF_INET,SOCK_STREAM,0);
        if(-1 == isock)
        {
            NgxLogStderr(errno,"CSocket::Initialize()'s Socket failed! portSeq = %d",i);
            return false;
        }

        //setsockopt() 设置一些套接字参数选项
        //参数2，表示级别，和参数3配套使用，
        //参数3,允许重用本地地址
        //设置 SO——REUSEADDR,
        int reuseaddr = 1;      //打开对应的设置
        if(setsockopt(isock, SOL_SOCKET, SO_REUSEADDR,(const void *)&reuseaddr, sizeof(reuseaddr)) == -1)
        {
            NgxLogStderr(errno, "CSocket::Initialize()'s setsockopt(SO_REUSEADDR) failed portSeq = %d", i);
            close(isock);
            return false;
        }
        //设置为非阻塞
        if(Setnonblocking(isock) == false)
        {
            NgxLogStderr(errno,"CSocket::Initialize()'s senoblocking() falied, portSeq = %d",i);
            close(isock);
            return false;
        }

        //设置本服务器要监听的地址和端口
        strInfo[0] = 0;
        sprintf(strInfo,"ListenPort%d",i);
        iport = pConfig->GetIntDefault(strInfo,10000);  //给这么个默认端口？
        servAddr.sin_port = htons((in_port_t)iport);

        if(-1 == bind(isock, (struct sockaddr*)&servAddr, sizeof(servAddr)))
        {
            NgxLogStderr(errno, "CSocket::INitialize() bind failed, portSeq = %d",i);
            close(isock);
            return false;
        }


        if(-1 == listen(isock, NGX_LISTEN_BACKLOG))
        {
            NgxLogStderr(errno,"CSocket::Intialize() listen failed, portSeq = %d",i);
            close(isock);
            return false;
        }

        //成功创建号套接字
        lpngxListeningT pListenSocketItem = new ngxListeningT;
        memset(pListenSocketItem,0,sizeof(ngxListeningT));
        pListenSocketItem->port = iport;
        pListenSocketItem->fd = isock;
        NgxLogErrorCore(NGX_LOG_INFO,0,"listen %d port successed!",iport);
        mListenSocketList.push_back(pListenSocketItem);
        
    }

    if(mListenSocketList.size() <= 0)
    {
        return false;
    }

    return true;
}

//设置为socket非阻塞模式
//不断调用，是否有数据过来，拷贝数据的时候是阻塞的
bool CSocket::Setnonblocking(int sockfd)
{
    int nb = 1; //0清除，1,设置
    if(ioctl(sockfd, FIONBIO, &nb) == -1)       //FIONBIO 设置和清除阻塞标记
    {
        return false;
    }
    return true;
}

//关闭socket
void CSocket::NgxCloseListeningSockets()
{
    for(int i = 0; i < mListenPortCount; ++i)
    {
        close(mListenSocketList[i]->fd);
        NgxLogStderr(NGX_LOG_INFO,0,"close listen port %d",mListenSocketList[i]->port);
    }
    return;
}

//将一个待发消息发送到消息队列中
void CSocket::MsgSend(char * pSendBuf)
{
    CLock lock(&mSendMessageQueueMutex);
    mMsgSendQueue.push_back(pSendBuf);
    ++miSendMsgQueueCount;

    // NgxLogStderr(0,"hereSend");

    //将信号量的值+1,这样其他卡在sem_wait的就可以走下去
    if(sem_post(&mSemEventSendQueue) == -1)
    {
        NgxLogStderr(0,"CSocket::MsgSend() sem_post(semEvent) failed");
    }

    return;
}


//主动关闭一个连接之后的处理函数
void CSocket::zdCloseSocketProc(lpngxConnectionT pConn)
{
    if(mifKickTimeCount == 1)
    {
        //从时间队列中删除
        DeteleFromTimeQueue(pConn);
    }
    if(-1 != pConn->fd)
    {
        close(pConn->fd);
        pConn->fd;
    }

    if(pConn->iThrowSendCount > 0)
    {
        --pConn->iThrowSendCount;
    }
    InRecyConnecQueue(pConn);
    return;
}


bool CSocket::TestFlood(lpngxConnectionT pConn)
{
    struct timeval sCurrTime;
    uint64_t iCurrTime;
    bool reco = false;

    gettimeofday(&sCurrTime,nullptr);
    iCurrTime = (sCurrTime.tv_sec * 1000 + sCurrTime.tv_usec / 1000);   //毫秒

    if((iCurrTime - pConn->FloodKickLastTime) < mFloodTimeInterval)
    {
        //发包太频繁了
        ++pConn->FloodAttackCount;
        pConn->FloodKickLastTime = iCurrTime;
    }
    else
    {
        //不频繁就恢复技术
        pConn->FloodAttackCount = 0;
        pConn->FloodKickLastTime = iCurrTime;
    }

    if(pConn->FloodAttackCount >= mFloodKickCount)
    {
        //表示可以踢这个人
        reco = true;
    }

    return true;
}



//-----------------------
//（1）epoll功能初始化，子进程进行，被NgxWorkerProcessInit()调用
int CSocket::NgxEpollInit()
{
    //（1）很多内核版本不处理epoll_create参数，只要参数大于0急性
    //创建一个epoll对象，创建了一个红黑树，创建了一个双向连表。
    mEpollHandle = epoll_create(mWorkerConnections);
    if(-1 == mEpollHandle)
    {
        NgxLogStderr(errno,"CSocket::NgxEpollInit()'s epoll_create() failed! ");
        exit(2);    //这个是致命问题，直接放，退出就行了
    }

    //（2）创建连接池【数组】，创建出来，这个东西用来处理所有的客户端连接
    InitConnection();
    // mConnectionN = mWorkerConnections;      //记录当前连接池连接总数
    // //连接池
    // mPConnections = new ngxConnectionT[mConnectionN];   //new 不可以是失败

    // int i = mConnectionN;
    // lpngxConnectionT next = nullptr;
    // lpngxConnectionT c = mPConnections;

    // do
    // {
    //     --i;        //怎么就搞一个尾插法

    //     c[i].data = next;
    //     c[i].fd = -1;           //这里主要是初始化
    //     c[i].instance = 1;      //标记位是1
    //     c[i].iCurrSequence = 0;

    //     next = &c[i];

    // }while(i);
    // mPFreeConnections = next;       //一开始的空闲链表肯定是表头，整个都是空的
    // mFreeConnectionN = mConnectionN;        //

    //(3)遍历监听所有socket【监听端口】，我们为每个监听socket增加一个连接池中的连接，
    //说白了就是让一个socket和一个内存绑定。
    std::vector<lpngxListeningT>::iterator pos;
    for(pos = mListenSocketList.begin(); pos != mListenSocketList.end(); ++pos)
    {

        lpngxConnectionT pConn = NgxGetConnection((*pos)->fd);

        if(pConn == nullptr)
        {
            NgxLogStderr(errno, "CSocket::NgxEpollInit()'s ngxGetConnection failed");
            exit(2);
        }
        pConn->listening = (*pos);
        (*pos)->connection = pConn;

        pConn->rHandler = &CSocket::NgxEventAccept;

        if(NgxEpollOperEvent((*pos)->fd,
                              EPOLL_CTL_ADD,        //事件类型
                              EPOLLIN | EPOLLRDHUP,
                              0,
                              pConn) == -1) //标记，这里代表要增加的标记，EPOLLIN 可读，EPOLLRDHUP，TCP连接的远端关闭或者半关闭
        {
            exit(2);
            //有问题
        }


        // c = NgxGetConnection((*pos)->fd);

        // if(c == nullptr)
        // {
        //     //这是致命问题
        //     NgxLogStderr(errno,"CSocket::NgxEpollInit() NgxGetConnection falied");
        //     exit(2);
        // }
        // c->listening = (*pos);  //连接对象和监听对象
        // (*pos)->connection = c; //监听对象连接连接池

        // c->rHandler = &CSocket::NgxEventAccept;      //对监听的读事件设置处理方法，

        //     //往监听socket上增加监听事件，从而开始让监听端口履行其职责，
        // if(-1 == NgxEpollAddEvent((*pos)->fd,1,0,0,EPOLL_CTL_ADD,c))
        // {
        //     exit(2);
        // }
    }
    return 1;
}

//(2)监听端口开始工作，
/**
 * fd 一个socket
 * readevent,表示读事件，0不是，1是
 * writeevent 表示写事件，0不是，1是
 * otherflag 其他需要补充的标记
 * eventtype 事件类型，一般就是枚举类型
 * c 对应连接池中的指针
 * 成功返回1,失败返回-1
*/
// int CSocket::NgxEpollAddEvent(int fd,
//                               int readevent, int writeevent,
//                               uint32_t otherflag,
//                               uint32_t eventtype,
//                               lpngxConnectionT c)
// {
//     struct epoll_event ev;
//     memset(&ev, 0, sizeof(ev));

//     if(1 == readevent)
//     {
//         ev.events = EPOLLIN | EPOLLRDHUP;
//         //epollin表示读事件，也就是read ready【客户端三次握手进来，也属于一种可读事件】
//         //epollrdhup 客户端关闭连接，断连
        
//     }
//     else
//     {
//         //暂时不写其他情况

//     }

//     if(otherflag != 0)
//     {
//         ev.events |= otherflag;
//     }

//     ev.data.ptr = (void *)((uintptr_t)c | c->instance);
//     //这行的目的是为了让socket和一块内存连接池绑定在一起。
//     //之后在使用的时候就可以快速获取这块内存
//     //这行代码做了两件事
//     //第一件事，ev.data.ptr = c,将来通过ev.data.ptr就能获取到socket所绑定的
//     //第二件事，c是一个指针，指针指向地址，但是也可以看成是数字，但是这个数字的最后一位一定不是1,一定是0,
//     //所以就用来保存c->instance这个位域变量，这个位域是1位
//     if(epoll_ctl(mEpollHandle,eventtype,fd,&ev) == -1)
//     {
//         NgxLogStderr(errno,"CSocket::NgxEpollAddEvent()'s epoll_ctl(%d,%d,%d,%u,%u) failed",fd,readevent,writeevent,otherflag,eventtype);
//         return -1;
//     }

//     return 1;
// }


//对epoll事件的具体操作
int CSocket::NgxEpollOperEvent(int fd,
                               uint32_t  eventType,
                               uint32_t  flag,
                               int      bcaction,
                               lpngxConnectionT pConn)
{
    struct epoll_event ev;
    memset(&ev,0,sizeof(ev));

    if(eventType == EPOLL_CTL_ADD)  //往红黑树中添加节点
    {
        // ev.data.ptr = (void *)pConn;
        ev.events = flag;       //既然增加节点，则不管原来是什么标记了
        pConn->events = flag;   //这个连接本身也记录这个标记
    }
    else if(eventType == EPOLL_CTL_MOD)
    {
        //节点已经在红黑数中，要修改节点中的信息
        ev.events = pConn->events;
        if(0 == bcaction)
        {
            //增加某个标记
            ev.events |= flag;
        }
        else if(1 == bcaction)
        {
            //去掉某个标记
            ev.events &= ~flag;
        }
        else
        {
            //完全覆盖某个标记
            ev.events = flag;
        }
        pConn->events = ev.events;
    }
    else
    {
        //删除红黑树的节点
        return 1;
    }

    ev.data.ptr = (void*)pConn;

    if(epoll_ctl(mEpollHandle, eventType, fd, &ev) == -1)
    {
        NgxLogStderr(errno, "CSocket::NgxEpollOperEvent() 's Epoll_CTL(%d,%ud,%ud,%d) failed",fd,eventType,flag,bcaction);
        return -1;
    }
    return 1;
}

//开始获取发生的事件消息
//参数unsigned int timer, epoll_wait()阻塞时长，单位是毫秒
//返回值，1，正常返回
int CSocket::NgxEPollProcessEvents(int timer)
{
    //等待事件，事件会返回到mEvents里，最多返回NGX_MAX_EVENTS个是事件
    //阻塞timer时间，除非1）阻塞时间到达2）阻塞期间收到事件会立即返回3）调用时有了事件立即返回4）或者来个信号
    //timer = -1一直阻塞，timer = 0 立即返回
    int events = epoll_wait(mEpollHandle,mEvents,NGX_MAX_EVENTS,timer); //返回的事件数目

    if(-1 == events)    //出现问题
    {
        //发送信号会导致这个判断条件成立
        if(errno == EINTR)
        {
            //这是信号所导致的，直接返回
            NgxLogErrorCore(NGX_LOG_INFO,errno,"CSocket::NgxEpollProcessEvents() epoll_wait failed!");
            return 1;   //正常返回
        }
        else
        {
            NgxLogErrorCore(NGX_LOG_ALERT,errno,"CSocket::NgxEpollProcessEvents() epoll_wait failed!");
            return 0;   //错误返回
        }
    }

    if(0 == events) //超时，但是事件没来
    {
        if(-1 != timer)
        {
            //这里是阻塞到事件了，就正常返回
            return 1;
        }
        //无限等待
        NgxLogErrorCore(NGX_LOG_ALERT,errno,"CSocket::NgxEpollProcessEvents time out but no events");
        return 0;
    }


    //会惊群，一个telnet上来，4个worker都会被惊动

    //走到这里，就是有事件收到了
    lpngxConnectionT pConn;
    // uintptr_t       instance;
    uint32_t        rEvents;
    for(int i = 0; i < events; ++i)
    {
        pConn = (lpngxConnectionT)(mEvents[i].data.ptr);
        // instance = (uintptr_t) c & 1;
        // c = (lpngxConnectionT)((uintptr_t) c & (uintptr_t) ~1);


        // if(-1 == c->fd) //关闭连接时，这个c->fd会变成-1
        // {
        //     NgxLogErrorCore(NGX_LOG_DEBUG,0,"CSocket::NgxEpollProcessEvents()'s fd = -1");
        //     continue;   //不去理会这种事件
        // }

        // if(c->instance != instance)
        // {
        //     //这个标记是用来表示该事件对应的套接字已经过期了
        //     NgxLogErrorCore(NGX_LOG_DEBUG,0,"CSocket::NgxEpollProcessEvents() instance out");
        //     continue;
        // }

        //处理正常事件
        rEvents = mEvents[i].events;
        // if(rEvents & (EPOLLERR | EPOLLHUP)) //发生了错误或者客户端断连
        // {
        //     rEvents |= EPOLLIN | EPOLLOUT;
        // }

        if(rEvents & EPOLLIN)   //如果是读事件,这里其实就是三次握手成功后会调用这个
        {
            (this->*(pConn->rHandler))(pConn);
            //新连接已经接入，那就是执行NgxEventAccept(c)
            //如果是已经连入，那就是执行NgxWaitRequestHandler(c)
        }

        if(rEvents & EPOLLOUT) //这里就是写事件了
        {
            //.....待扩展
            // NgxLogStderr(errno,"22222222222222222222.");
            if(rEvents & (EPOLLERR | EPOLLHUP | EPOLLRDHUP))
            {
                //三个错误分别是
                //对应的连接发生错误
                //对应的连接被挂起
                //TCP连接关闭或者是半关闭
                --pConn->iThrowSendCount;
            }
            else
            {
                (this->*(pConn->wHandler))(pConn);
            }

        }
    }

    return 1;

}

//处理发消息队列的函数
void * CSocket::ServerSendQueueThread(void * threadData)
{
    ThreadItem * pThread = static_cast<ThreadItem*>(threadData);
    CSocket * pSocketObj = pThread->_pThis;
    int err;

    std::list<char *>::iterator pos, pos2, posend;

    char * pMsgBuf;
    LPSTRUC_MSG_HEADER  pMsgHeader;
    LPCOMM_PKG_HEADER   pPkgHeader;
    lpngxConnectionT    pConn;
    unsigned short      itemp;
    ssize_t             sendSize;

    CMemory * pMemory = CMemory::GetInstance();

    while(0 == gStopEvent)
    {
        //如果信号量>0，则-1,并走下去，否则就是在这里卡这【为了让信号量+1,可以在其他线程中调用sem_post达到】
        //。。。。。。如果被某个信号中断，sem_wait也可能过早返回，错误为EINTR
        //整个程序退出之前，也要sem_post一下，确保如果本线程卡在sem_wait()，也能走下去让本线程成功返回
        if(sem_wait(&pSocketObj->mSemEventSendQueue) == -1)
        {
            //错误就及时报告
            if(errno == EINTR)
            {
                NgxLogStderr(errno,"CSocket::ServerSendQueueThread() sem_wait() failed");
            }
        }

        //然后走到这里都要处理数据收发
        if(gStopEvent != 0)
        {
            break;      //退出整个进程
        }

        if(pSocketObj->miSendMsgQueueCount > 0)
        {
            err = pthread_mutex_lock(&pSocketObj->mSendMessageQueueMutex);
            if(0 != err)
            {
                NgxLogStderr(err,"CSocket::ServerSendQueueThread() 's pthread_mutex_lock() failed");
            }

            pos = pSocketObj->mMsgSendQueue.begin();
            posend  = pSocketObj->mMsgSendQueue.end();

            while(pos != posend)
            {
                pMsgBuf = (*pos);       //消息包括了【消息头+包头+包体】，但是给客户端不发送消息头
                pMsgHeader = (LPSTRUC_MSG_HEADER)pMsgBuf;
                pPkgHeader = (LPCOMM_PKG_HEADER)(pMsgBuf + pSocketObj->miLenMsgHeader);
                pConn = pMsgHeader->pConn;


                //检查包过期
                if(pConn->iCurrSequence != pMsgHeader->iCurrsequence)
                {
                    //本包保存的序列号与pConn【连接池中连接】中实际的序列号已经不同，丢弃此消息，
                    pos2 = pos;
                    ++pos;
                    pSocketObj->mMsgSendQueue.erase(pos2);
                    --pSocketObj->miSendMsgQueueCount;
                    pMemory->FreeMemory(pMsgBuf);
                    continue;
                }

                if(pConn->iThrowSendCount > 0)
                {
                    //靠系统驱动来发送消息，所以这里不发送
                    ++pos;
                    continue;
                }

                //走到这里，可以发送消息，一些必须的信息记录，
                pConn->pSendMemPoint = pMsgBuf;
                pos2 = pos;
                ++pos;
                pSocketObj->mMsgSendQueue.erase(pos2);
                --pSocketObj->miSendMsgQueueCount;      //发送消息的队列就要减1
                pConn->pSendBuf = (char*)pPkgHeader;    //要发送的数据的缓冲区指针，因为发送数据不一定全部能发送出去，
                itemp = ntohs(pPkgHeader->pkgLen);      //包头+包体
                // NgxLogStderr(0,"socketSendServer-pkglen:%d",itemp);
                pConn->iSendLen = itemp;

                //我们采用epoll水平触发的策略，所以能够走到这里的，都应该是还没有投递 写事件 到 epoll中
                //开始不把socket写事件通知加入到epoll，当我们需写数据的时候，直接调用write或者send发送数据
                //如果返回了EAGIN[缓冲区满了]，再把写事件通知到epoll中
                

                //（1）直接调用write或者send发送数据
                NgxLogStderr(errno,"ready to send data %ud.",pConn->iSendLen);

                sendSize = pSocketObj->SendProc(pConn,pConn->pSendBuf,pConn->iSendLen);
                // NgxLogStderr(0,"sendSize::%d",sendSize);
                if(sendSize > 0)
                {
                    if(sendSize == pConn->iSendLen)
                    {
                        //成功发送出数据
                        pMemory->FreeMemory(pConn->pSendMemPoint);
                        pConn->pSendMemPoint = nullptr;
                        pConn->iThrowSendCount = 0;
                        NgxLogStderr(0,"CSocket::ServerSendQueueThread()'s data sent over!");
                    }
                    else
                    {
                        //这里就是没有全部发送完毕
                        //因为是发送缓冲区满了
                        pConn->pSendBuf = pConn->pSendBuf + sendSize;
                        pConn->iSendLen = pConn->iSendLen - sendSize;
                        ++pConn->iThrowSendCount;
                        if(pSocketObj->NgxEpollOperEvent(pConn->fd,
                                                         EPOLL_CTL_MOD,
                                                         EPOLLOUT,
                                                         0,
                                                         pConn) == -1)
                        {
                            NgxLogStderr(errno,"CSocket::ServerSendQueueThread()'s ngxEpollOperEvent failed");
                        }

                        // NgxLogStderr(errno,"CSocket::SererSendQueueThread()'s data sent remain, sent %d len, remain %d, len",sendSize,pConn->iSendLen);
                    }

                    continue;
                }
                else if(0 == sendSize)
                {
                    pMemory->FreeMemory(pConn->pSendMemPoint);
                    pConn->pSendMemPoint = nullptr;
                    pConn->iThrowSendCount = 0;
                    continue;
                }
                else if(-1 == sendSize)
                {
                    //一个字节都没发出去，说明此时缓冲区真好是满的
                    ++pConn->iThrowSendCount;
                    if(pSocketObj->NgxEpollOperEvent(pConn->fd,
                                                        EPOLL_CTL_MOD,
                                                        EPOLLOUT,
                                                        0,
                                                        pConn) == -1)
                    {
                        NgxLogStderr(errno,"CSocket::ServerSendQueueThread()'s ngxEpollOperEvent 2 failed");
                    }
                    continue;
                }
                else
                {
                    //断开的情况
                    pMemory->FreeMemory(pConn->pSendMemPoint);
                    pConn->pSendMemPoint = nullptr;
                    pConn->iThrowSendCount = 0;
                    continue;
                }
            }

            err = pthread_mutex_unlock(&pSocketObj->mSendMessageQueueMutex);
            if(0 != err)
            {
                NgxLogStderr(err,"CSocket::ServerSendQueueThread()'s pthread_mutex_unlock() failed, return error value is %d",err);
            }
        }
    }

    return (void*)0;

}