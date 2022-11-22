#ifndef __NGX_SOCKET_H__
#define __NGX_SOCKET_H__

#include <vector>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <list>
#include <atomic>
#include <pthread.h>
#include <semaphore.h>
#include <map>

#include "ngx_comm.h"

#define NGX_LISTEN_BACKLOG  511      //已完成连接队列，nginx给511,
#define NGX_MAX_EVENTS      512


typedef struct NgxListeningS ngxListeningT, *lpngxListeningT;;
typedef struct NgxConnectionS ngxConnectionT, *lpngxConnectionT;
typedef class CSocket       CSocket;

typedef void (CSocket::*ngxEventHandlerPT)(lpngxConnectionT c);     //定义一个函数指针

struct NgxListeningS
{
    int         port;       //监听的端口号
    int         fd;         //套接字句柄
    lpngxConnectionT connection;    //连接池中的一个连接，是一个指针
};


//（1）该结构表示一个TCP连接，【客户端主动发起的，Nginx服务器被动接受的TCP连接】
struct NgxConnectionS
{

    NgxConnectionS();       //构造函数
    virtual ~NgxConnectionS();
    void GetOneToUse();         
    void PutOneToFree();

    int                 fd;
    lpngxListeningT     listening;      //如果这个链接被分配了一个监听套接字，那么这个里边就指向那个内存的首地址

    //--------------------
    // unsigned            instance:1;    //【位域】就是有多少位二进制，0表示有效，1表示实效
    uint64_t            iCurrSequence;      //引入的一个序号，每次分配出去+1，可以检测废包
    struct sockaddr     sSockAddr;          //保存对方地址信息

    //和读有关的标志
    // uint8_t             wReady;         //写标记

    ngxEventHandlerPT   rHandler;           //读相关的处理方法
    ngxEventHandlerPT   wHandler;           //写事件的处理方法


    //和epoll事件有关
    uint32_t                events;


    //和收包有关
    unsigned char           curStat;            //当前收包的状态
    char                    dataHeadInfo[_DATA_BUFSIZE_]; //用于保存收到的数据包头的信息
    char                    *precvbuf;          //接受数据缓冲区的头指针，对收到不全的包非常有用
    char                    irecvLen;           //要收到多少数据，由这个变量指定
    char                    *precvMemPointer;     //new出来用于收包的内存首地址，和上一个变量配对使用

    // bool                    ifnewrecvMem;       //如果我们成功的收到了包头，那么我们就要分配内存开始保存，包头+消息头+包体内容

    //和发包有关
    std::atomic<int>        iThrowSendCount;    //发送消息
    char                    *pSendMemPoint;     //发送完成释放用的
    char                    *pSendBuf;          //发送数据缓冲区的指针
    unsigned                iSendLen;           //要发的数据

    //逻辑处理的互斥量
    pthread_mutex_t         logicProcMutex;    


    //和回收有关
    time_t                  inRecyTime;


    //和心跳包有关
    time_t                  lastPingTime;


    //和网络安全有关
    uint64_t                FloodKickLastTime;          //flood攻击上次收到包的时间
    int                     FloodAttackCount;           //flood攻击在该时间内收到包的次数统计

    //---------------------
    lpngxConnectionT    next;           //这是一个后继指针

};

//消息头，引入目的是当收到数据包时，额外记录一些内容以备将来使用
typedef struct _STRUC_MSG_HEADER
{
    lpngxConnectionT pConn;
    uint64_t        iCurrsequence;      //收到数据包后记录对应连接的序号，将来能用于比较连接已经作废
} STRUC_MSG_HEADER, *LPSTRUC_MSG_HEADER;






//socket相关
class CSocket
{
    public:
        CSocket();
        virtual ~CSocket();
        virtual bool Initialize();
        virtual bool InitializeSubProc();       //初始化子进程
        virtual void ShutDownSubProc();         //关闭退出子进程

        // void PrintTDInfo();             //打印各种消息

        int NgxEpollInit();         //epoll功能初始化

        //处理事件
        // int NgxEpollAddEvent(int fd, int readevent, int writeevent, uint32_t otherflag, uint32_t eventtype, lpngxConnectionT c);
        //等待和接受事件
        int NgxEPollProcessEvents(int timer);
        //epoll操作事件
        int NgxEpollOperEvent(int fd, uint32_t eventType, uint32_t flag, int bcaction, lpngxConnectionT pConn);

        // char * outMsgRecvQueue();           //将一个消息队列取出来
        virtual void ThreadRecvProcFunc(char * pMsgBug); //处理客户端请求
        virtual void ProcPingTimeOutChecking(LPSTRUC_MSG_HEADER tempMsg, time_t curTime);
        

    protected:
        //数据发送相关
        void MsgSend(char * pSendBuf);
        void zdCloseSocketProc(lpngxConnectionT pConn);     //主动关闭一个连接时要做些善后的处理工作

        //一些和网络通信有关的
        size_t                  miLenPkgHeader;     //包头+长度
        size_t                  miLenMsgHeader;     //消息头长度

        int                     miWaitTime;         //多少秒检测一次
        int                     miFTimeOutKick;     



    private:

        struct ThreadItem
        {
            pthread_t       _Handle;
            CSocket         *_pThis;
            bool            ifrunning;      //标记是否启动起来

            ThreadItem(CSocket * pthis) : _pThis(pthis), ifrunning(false) {}
            ~ThreadItem() {}
        };

        std::vector<lpngxListeningT> mListenSocketList; //监听套接字队列
        struct epoll_event           mEvents[NGX_MAX_EVENTS];

        int         mWorkerConnections;
        int         mListenPortCount;
        int         mEpollHandle;       //epoll_create返回的句柄


        //和连接池有关系的
        // lpngxConnectionT         mPConnections;      //这是连接池的首地址
        // lpngxConnectionT         mPFreeConnections;  //空闲连接链表头，连接池中总是有些连接被占用
        //                                             //为了快速在池中找到一个空闲的连接，
        std::list<lpngxConnectionT>         mConnectionList;        //连接列表
        std::list<lpngxConnectionT>         mFreeConnectionList;    //空闲连接列表
        std::atomic<int>                    mTotalConnectionN;      //连接池总数
        std::atomic<int>                    mFreeConnectionN;       //空闲连接池总数
        pthread_mutex_t                     mConnctionMutex;        //连接相关互斥量
        pthread_mutex_t                     mRecyConnQueueMutex;    //连接回收相关的互斥量
        std::list<lpngxConnectionT>         mRecyConnectionList;    //将要释放的放在这里
        std::atomic<int>                    mTotolRecyConnectionN;   //待释放的连接大小
        int                                 mRecyConnectionWaitTime;//等待回收的时间


        // int                     mConnectionN;       //当前进程中所有连接对象的总数
        // int                     mFreeConnectionN;      //可用连接总数


        //消息队列
        std::list<char*>        mMsgSendQueue;      //消息队列
        // int                     miRecvMsgQueueCount;        //收消息队列
        std::atomic<int>        miSendMsgQueueCount;            //发消息队列大小

        //多线程相关
        // pthread_mutex_t         mRecvMessageQueueMutex; //收消息队列互斥量
        std::vector<ThreadItem*>    mThreadVector;          //线程容器
        pthread_mutex_t                   mSendMessageQueueMutex;     //发消息队列信号量
        sem_t                       mSemEventSendQueue;         //处理发消息线程相关的量


        //时间相关
        int                         mifKickTimeCount;       //是否开启踢人，1 开启
        pthread_mutex_t             mTimeQueueMutex;
        std::multimap<time_t, LPSTRUC_MSG_HEADER>   mTimeQueueMap;  //时间队列
        size_t                      mCurSize;                   //时间队列的大小
        time_t                      mTimerValue;                //当前计时队列头部值


        //在线用户个相关
        std::atomic<int>            mOnlineUserCount;           //在线用户统计

        //网络安全相关
        int                         mFloodAttackEnable;         //攻击检测是否开启
        unsigned int                mFloodTimeInterval;         //表示每次收到包的间隔
        int                         mFloodKickCount;            //踢出的人数累积

        //统计
        time_t                      mLastPrintTime;
        int                         miDiscardSendPkgCount;      //丢弃的发送数据包 


        void ReadConf();                    //专门用于读取各种配置的
        bool NgxOpenListeningSockets();     //监听必须的端口
        void NgxCloseListeningSockets();        //关闭监听套接字
        bool Setnonblocking(int sockfd);        //设置非阻塞套接字

        //一些业务处理函数
        void NgxEventAccept(lpngxConnectionT oldc);      //建立新连接
        void NgxReadRequestHandler(lpngxConnectionT pConn); //设置数据来时的读取函数
        void NgxWriteRequestHandler(lpngxConnectionT pConn);
        // void NgxCloseAcceptedConnection(lpngxConnectionT c);        //用户连入
        void NgxCloseConnection(lpngxConnectionT pConn);

        ssize_t RecvProc(lpngxConnectionT c, char * buff, ssize_t buflen);      //专门接受从客户端来的数据
        void NgxWaitRequestHandlerProcPL(lpngxConnectionT pConn, bool &ifFlood);       //包头收完之后的处理
        void NgxWaitRequsetHandlerProcLast(lpngxConnectionT pConn, bool &ifFlood); //整个包收完的处理
        void ClearMsgSendQueue();                               //处理消息发送队列

        ssize_t SendProc(lpngxConnectionT c, char * buff, ssize_t size);        //将数据发送到客户端
        // void InMsgRecvQueue(char * buf, int & irmqc);            //收到一个完整消息后，加入消息队列
        // void tempOutMsgRecvQueue();     //临时删除消息队列中的元素
        // void ClearMsgRecvQueue();      //清空消息队列



        //获取对端信息
        size_t NgxSockNtop(struct sockaddr * sa, int port, u_char *text, size_t len);

        //和连接池有关
        void InitConnection();
        void ClearConnection();
        lpngxConnectionT NgxGetConnection(int isock);           //从连接池中获取一个空闲连接
        void NgxFreeConnection(lpngxConnectionT pConn);             //归还连接池
        void InRecyConnecQueue(lpngxConnectionT pConn);

        //和时间相关的函数
        void AddToTimeQueue(lpngxConnectionT pConn);            //设置踢出时钟
        time_t  GetEarliestTime();                              //获取最最早的时间
        LPSTRUC_MSG_HEADER RemovFirstTimer();                   //移除最早的时间
        LPSTRUC_MSG_HEADER GetOverTimeTimer(time_t curTime);    //从队列中找更早的时间
        void DeteleFromTimeQueue(lpngxConnectionT pConn);       //把指定用户tcp连接移除
        void ClearAllFromTimerQueue();                          //清空时间队    

        //和网络安全有关
        bool TestFlood(lpngxConnectionT pConn);                 //测试攻击是否成立

        //线程函数相关
        static void * ServerRecyConnectionThread(void * threadData); //专门用来回收连接的线程
        static void * ServerSendQueueThread(void * threadData); //用来专门发送数据的线程
        static void * ServerTimeQueueMonitorThread(void * threadData);

        

};

#endif