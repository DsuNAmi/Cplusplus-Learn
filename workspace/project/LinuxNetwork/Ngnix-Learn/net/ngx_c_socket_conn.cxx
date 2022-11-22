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
#include <sys/ioctl.h>
#include <arpa/inet.h>




#include "ngx_c_conf.h"
#include "ngx_macro.h"
#include "ngx_global.h"
#include "ngx_func.h"
#include "ngx_c_socket.h"
#include "ngx_c_memory.h"
#include "ngx_c_lockmutex.h"


//构造函数
NgxConnectionS::NgxConnectionS()
{
    iCurrSequence = 0;
    pthread_mutex_init(&logicProcMutex,nullptr);
}

NgxConnectionS::~NgxConnectionS()
{
    pthread_mutex_destroy(&logicProcMutex);
}


//分配一个连接的时候初始化一些内容
void NgxConnectionS::GetOneToUse()
{
    ++iCurrSequence;

    fd = -1;
    curStat = _PKG_HD_INIT;
    precvbuf = dataHeadInfo;
    irecvLen = sizeof(COMM_PKG_HEADER);

    precvMemPointer = nullptr;
    iThrowSendCount = 0;
    pSendMemPoint = nullptr;
    events = 0;
    lastPingTime = time(NULL);


    FloodKickLastTime = 0;
    FloodAttackCount = 0;

}

//回收一个连接池时候做得一些事
void NgxConnectionS::PutOneToFree()
{
    ++iCurrSequence;
    if(precvMemPointer != nullptr)
    {
        CMemory::GetInstance()->FreeMemory(precvMemPointer);
        precvMemPointer = nullptr;
    }
    if(pSendMemPoint != nullptr)
    {
        CMemory::GetInstance()->FreeMemory(pSendMemPoint);
        pSendMemPoint = nullptr;
    }


    iThrowSendCount = 0;
}


//初始化连接池
void CSocket::InitConnection()
{
    lpngxConnectionT pConn;
    CMemory * pMemory = CMemory::GetInstance();

    int iLenConnPool = sizeof(ngxConnectionT);
    //先创建一定数量的连接,之后有扩容需求再增加
    for(int i = 0; i < mWorkerConnections; ++i)
    {
        pConn = (lpngxConnectionT)pMemory->AllocMemory(iLenConnPool, true);//这里写true需要初始化
        //手工调用构造函数,因为AllocMemory无法调用构造函数
        pConn = new(pConn) ngxConnectionT();
        pConn->GetOneToUse();
        mConnectionList.push_back(pConn);       //这个是所有的连接,不关心是否空闲
        mFreeConnectionList.push_back(pConn);
    }

    mFreeConnectionN = mTotalConnectionN = mConnectionList.size();
    return;
}

//最终回收连接池
void CSocket::ClearConnection()
{
    lpngxConnectionT pConn;
    CMemory * pMemory = CMemory::GetInstance();
    while(!mConnectionList.empty())
    {
        pConn = mConnectionList.front();
        mConnectionList.pop_front();
        pConn->~NgxConnectionS();
        pMemory->FreeMemory(pConn);
    }
}



lpngxConnectionT CSocket::NgxGetConnection(int isock)
{
    //因为可能有其他线程要访问list,所以需要上锁
    CLock lock(&mConnctionMutex);

    if(!mFreeConnectionList.empty())
    {
        //如果有空闲的连接池,那么就从空闲的连接池中取进行分配
        lpngxConnectionT pConn = mFreeConnectionList.front();
        mFreeConnectionList.pop_front();
        pConn->GetOneToUse();
        --mFreeConnectionN;
        pConn->fd = isock;
        return pConn;
    }

    //这说明没有空闲的连接了
    CMemory * pMemory = CMemory::GetInstance();
    lpngxConnectionT pConn = (lpngxConnectionT)pMemory->AllocMemory(sizeof(ngxConnectionT),true);
    pConn = new(pConn) ngxConnectionT();
    pConn->GetOneToUse();
    mConnectionList.push_back(pConn);
    ++mTotalConnectionN;
    pConn->fd = isock;
    return pConn;

    // lpngxConnectionT c = mPFreeConnections;

    // if(c == nullptr)
    // {
    //     NgxLogStderr(0,"CSocket::NgxGetConnection() free links is empty!");
    //     return nullptr;
    // }

    // mPFreeConnections = c->data;
    // --mFreeConnectionN;

    // //(1)注意这里的操作，先把c指向的对象中有用的东西保存成变量
    // uintptr_t instance = c->instance;   //默认值是1，表示无效
    // uint64_t iCurrSequence = c->iCurrSequence;

    // //(2)清空数据，之后给值
    // memset(c,0,sizeof(ngxConnectionT));
    // c->fd = isock;
    // c->curStat = _PKG_HD_INIT;          //所有刚连接的都是初始状态
    
    // c->precvbuf = c->dataHeadInfo;      //把指针接到缓冲区的头，因为也是从头部开始收包
    // c->irecvLen = sizeof(COMM_PKG_HEADER); //一个包头的长度，就是要接受的长度

    // c->ifnewrecvMem = false;
    // c->pneMemPointer = nullptr;


    // c->instance = !instance;
    // c->iCurrSequence = iCurrSequence; ++c->iCurrSequence;   //每次用都加一

    // return c;
}

void CSocket::NgxFreeConnection(lpngxConnectionT pConn)
{

    CLock lock(&mConnctionMutex);

    //首先明确一点,连接,所有连接全部都在List里
    pConn->PutOneToFree();

    //扔到空闲的连接池list里
    mFreeConnectionList.push_back(pConn);
    ++mFreeConnectionN;
    return;

    // if(c->ifnewrecvMem == true)
    // {
    //     CMemory::GetInstance()->FreeMemory(c->pneMemPointer);
    //     c->pneMemPointer = nullptr;
    //     c->ifnewrecvMem = false;
    // }

    // c->data = mPFreeConnections;

    // ++c->iCurrSequence;

    // mPFreeConnections = c;
    // ++mFreeConnectionN;
    // return;
}

//将要回收的连接放到一个队列中来,后续有专门的线程会处理这个函数队列中的回收
//有些连接并不希望马上释放
void CSocket::InRecyConnecQueue(lpngxConnectionT pConn)
{
    // NgxLogStderr(0,"CSocket::InRecyConnectQueue execute, and recyList");

    std::list<lpngxConnectionT>::iterator pos;
    bool iffind = false;

    CLock lock(&mRecyConnQueueMutex);

    //判断防止连接多次被扔到回收站中来
    for(pos = mRecyConnectionList.begin(); pos != mRecyConnectionList.end(); ++pos)
    {
        if((*pos) == pConn)
        {
            iffind = true;
            break;
        }
    }

    if(iffind == true)
    {
        return;
    }

    pConn->inRecyTime = time(NULL);
    ++pConn->iCurrSequence;
    mRecyConnectionList.push_back(pConn);
    ++mTotolRecyConnectionN;
    --mOnlineUserCount;         //连入用户数量-1
    return;
}

//我们用一个专门的线程来做回收这件事
void *CSocket::ServerRecyConnectionThread(void * threadData)
{
    ThreadItem * pThread = static_cast<ThreadItem*>(threadData);
    CSocket * pSocketObj = pThread->_pThis;

    time_t curTime;
    int err;
    std::list<lpngxConnectionT>::iterator pos,posend;
    lpngxConnectionT pConn;

    while(1)
    {
        //为了简化问题,我们直接每次休息200毫秒
        usleep(200 * 1000);

        //不管什么情况,先把这个条件成立时该做的动作做了
        if(pSocketObj->mTotolRecyConnectionN > 0)
        {
            curTime = time(NULL);
            err = pthread_mutex_lock(&pSocketObj->mRecyConnQueueMutex);
            if(0 != err)
            {
                NgxLogStderr(err,"CSocket::ServerRecyThread() 's pthread_mutex_lock() failed");
            }
lblRRTD:    
            pos = pSocketObj->mRecyConnectionList.begin();
            posend = pSocketObj->mRecyConnectionList.end();
            for(; pos != posend; ++pos)
            {
                pConn = (*pos);
                if(((pConn->inRecyTime + pSocketObj->mRecyConnectionWaitTime) > curTime) && (gStopEvent == 0))
                {
                    //还没有到释放的时间
                    continue;
                }


                if(pConn->iThrowSendCount != 0)
                {
                    NgxLogStderr(0,"CSocket::ServerRecyConnectionThread() find iThrowSendCount = 1");
                }

                //到了释放的时间,那么就可以释放了
                --pSocketObj->mTotolRecyConnectionN;
                pSocketObj->mRecyConnectionList.erase(pos);

                NgxLogStderr(0,"CSocket::ServerRecyThread() execute, connetion %d is realese",pConn->fd);

                pSocketObj->NgxFreeConnection(pConn);
                goto lblRRTD;
            }

            err = pthread_mutex_unlock(&pSocketObj->mRecyConnQueueMutex);
            if(0 != err)
            {
                NgxLogStderr(0,"CSocket::ServerRecyThread() pthread_mutex_unlock() failed");
            }
        }

        //如果整个程序要退出,那么就要退出这个循环
        if(1 == gStopEvent)
        {
            if(pSocketObj->mTotolRecyConnectionN > 0)
            {
                //因为要退出,所以就不用关时间到没到,直接释放就完事了
                err = pthread_mutex_lock(&pSocketObj->mRecyConnQueueMutex);
                if(0 != err)
                {
                    NgxLogStderr(0,"CSocket::ServerRecyThread() pthread_mutex_lock() 2 failed");
                }

        lblRRTD2:
                pos  = pSocketObj->mRecyConnectionList.begin();
                posend = pSocketObj->mRecyConnectionList.end();
                for(; pos != posend; ++pos)
                {
                    pConn = (*pos);
                    --pSocketObj->mTotolRecyConnectionN;
                    pSocketObj->mRecyConnectionList.erase(pos);
                    pSocketObj->NgxFreeConnection(pConn);
                    goto lblRRTD2;
                }
                err = pthread_mutex_unlock(&pSocketObj->mRecyConnQueueMutex);
                if(0 != err)
                {
                    NgxLogStderr(0,"CSocket::ServerRecyThread() pthread_mutex_unlock() 2 failed");
                }

            }
            break;
        }
    }

    return (void*)0;
}


void CSocket::NgxCloseConnection(lpngxConnectionT pConn)
{

    NgxFreeConnection(pConn);

    if(close(pConn->fd) == -1)
    {
        NgxLogErrorCore(NGX_LOG_ALERT,errno, "CSocket::NgxCloseConnection() close(%d) failed",pConn->fd);
    }
    return;

    // if(close(c->fd) == -1)
    // {
    //     NgxLogErrorCore(NGX_LOG_ALERT,errno,"CSocket::NgxCloseConnection() close(%d) failed",c->fd);
    // }
    // c->fd = -1;
    // NgxFreeConnection(c);
    // return;

}
