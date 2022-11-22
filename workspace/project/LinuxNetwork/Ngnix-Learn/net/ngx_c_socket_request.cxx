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
#include <pthread.h>

#include "ngx_c_conf.h"
#include "ngx_macro.h"
#include "ngx_global.h"
#include "ngx_func.h"
#include "ngx_c_socket.h"
#include "ngx_c_memory.h"
#include "ngx_c_lockmutex.h"

//来数据时候的处理，连上服务器有数据的时候处理
void CSocket::NgxReadRequestHandler(lpngxConnectionT pConn)
{
    
    bool isFlood = false;       //是否flood攻击

    ssize_t reco = RecvProc(pConn,pConn->precvbuf,pConn->irecvLen); //这个收包的长度非常关键，不然后面的代码都是错的
    // NgxLogStderr(0,"reahand:pkglen:%d",reco);
    if(reco <= 0)
    {
        return;
    }

    //这里就是成功收到了一些字节
    if(pConn->curStat == _PKG_HD_INIT) //起始的状态
    {
        if(reco == miLenPkgHeader)
        {
            //正好长度和包头的的大小一样，那么就拆解包头
            NgxWaitRequestHandlerProcPL(pConn,isFlood);
        }
        else 
        {
            //收到的包不完整
            pConn->curStat = _PKG_HD_RECVING;
            pConn->precvbuf = pConn->precvbuf + reco;
            pConn->irecvLen = pConn->irecvLen - reco;
        }
    }
    else if(pConn->curStat == _PKG_HD_RECVING)
    {
        //剩下的收完了
        if(pConn->irecvLen == reco)
        {
            NgxWaitRequestHandlerProcPL(pConn,isFlood);
        }
        else
        {
            //继续收
            pConn->precvbuf = pConn->precvbuf + reco;
            pConn->irecvLen = pConn->irecvLen - reco;           //不会是负的，限定了收到的长度
        }
    }
    else if(pConn->curStat == _PKG_BD_INIT)
    {
        //包头收完了，收包体
        if(reco == pConn->irecvLen)
        {
            if(mFloodAttackEnable == 1)
            {
                isFlood = TestFlood(pConn);
            }
            NgxWaitRequsetHandlerProcLast(pConn,isFlood);//包收完了
        }
        else
        {
            //接着收
            pConn->curStat = _PKG_BD_RECVING;
            pConn->precvbuf = pConn->precvbuf + reco;
            pConn->irecvLen = pConn->irecvLen - reco;
        }
    }
    else if(pConn->curStat == _PKG_BD_RECVING)
    {
        if(pConn->irecvLen == reco)
        {
            if(1 == mFloodAttackEnable)
            {
                isFlood = TestFlood(pConn);
            }
            NgxWaitRequsetHandlerProcLast(pConn,isFlood);
        }
        else
        {
            pConn->precvbuf = pConn->precvbuf + reco;
            pConn->irecvLen = pConn->irecvLen - reco;
        }
    }


    if(isFlood == true)
    {
        NgxLogStderr(errno,"alert the flood attack, kill the client");
        zdCloseSocketProc(pConn);
    }

    return;


    // NgxLogStderr(errno,"22222222222222222222");
    // unsigned char buf[10] = {0};
    // memset(buf,0,sizeof(buf));


    // int n = recv(c->fd,buf,2,0);
    // NgxLogStderr(0,"ok , the recv content bytes is %d, content is %s,",n,buf);

    
    // int n = recv(c->fd,buf,2,0);        //这里只接受两字节
    // for(int i = 0; i < 10; ++i)
    // {
    //     int n = recv(c->fd,buf,2,0);
    //     NgxLogStderr(0,"ok , the recv content bytes is %d, content is %s,",n,buf);
    // }

    // for(int i = 0; i < 10; ++i)
    // {
    //     int n = recv(c->fd,buf,2,0);
    //     if(-1 == n && errno == EAGAIN)
    //     {
    //         break;
    //     }
    //     NgxLogStderr(0,"ok , the recv content bytes is %d, content is %s,",n,buf);
    // }
    
    
    
    return;
}


//buf 接受数据的缓冲区
//return value > 0 ，表示接收到的字节数
ssize_t CSocket::RecvProc(lpngxConnectionT c, char * buff, ssize_t buflen)
{
    ssize_t n;
    n = recv(c->fd, buff, buflen, 0);
    if(0 == n)
    {
        //客户端关闭了
        // if(close(c->fd) == -1)
        // {
        //     NgxLogErrorCore(NGX_LOG_ALERT, errno, "CSocket::RecvProc() close(%d) failed",c->fd);
        // }
        // NgxCloseConnection(c);
        // InRecyConnecQueue(c);
        zdCloseSocketProc(c);
        return -1;
    }

    //如果发生了对应的错误
    if(n < 0)
    {
        //表示没有收到数据
        if(errno == EAGAIN || errno == EWOULDBLOCK)
        {
           NgxLogStderr(errno, "CSocket::RecvProc()'s errno == EAGIN or EWOULDBLOCK eastablished!");
           return -1; 
        }
        //当阻塞于某个慢系统调用的一个进程捕获某个信号且性相应信号处理函数返回时，
        if(errno == EINTR)
        {
            NgxLogStderr(errno,"CSocket::RecvProc()'s errno == EINTR");
            return -1;
        }
        
        //下面的错误都是异常，所以我们要关闭连接池
        //意外关闭的，不是走四次👋
        if(errno == ECONNRESET)
        {
            //DO NOTHING
        }
        else 
        {
            //其他错误
            NgxLogStderr(errno,"CSocket::RecvProc() error!,");
        }

        // // NgxCloseConnection(c);
        // if(close(c->fd) == -1)
        // {
        //     NgxLogErrorCore(NGX_LOG_ALERT,errno,"CSocket::RecvProc() close2(%d)",c->fd);

        // }
        // InRecyConnecQueue(c);
        zdCloseSocketProc(c);
        return -1;
    }

    //没问题，那就返回收到的字节数
    return n;
}

//我们来拆解包头
void CSocket::NgxWaitRequestHandlerProcPL(lpngxConnectionT pConn, bool &isFlood)
{
    CMemory * pMemory = CMemory::GetInstance();

    LPCOMM_PKG_HEADER pPkgHeader;
    //取出包头信息
    pPkgHeader = (LPCOMM_PKG_HEADER)pConn->dataHeadInfo;

    unsigned short ePkgLen = ntohs(pPkgHeader->pkgLen); //网络序转主机序

    //恶意包或者是错误包的判断
    if(ePkgLen < miLenPkgHeader)
    {
        //整个包长度比包头还要小的情况
        pConn->curStat = _PKG_HD_INIT;
        pConn->precvbuf = pConn->dataHeadInfo;
        pConn->irecvLen = miLenPkgHeader;
    }
    else if(ePkgLen > (_PKG_MAX_LENGTH - 1000)) //包太长了
    {
        pConn->curStat = _PKG_HD_INIT;
        pConn->precvbuf = pConn->dataHeadInfo;
        pConn->irecvLen = miLenPkgHeader;
    }
    else
    {
        //正常处理包头
        //因为包体的长度不固定，所以要分配内存来收包体
        char * pTempBuff = (char *)pMemory->AllocMemory(miLenMsgHeader + ePkgLen, false);
        //长度是消息头+包头+包体，首先不需要memset
        // c->ifnewrecvMem = true;     //表示之后要回收内存
        pConn->precvMemPointer= pTempBuff;       //这里是指到接受的包+消息头的头部

        //先构造一个消息头
        LPSTRUC_MSG_HEADER pTempMsgHeader = (LPSTRUC_MSG_HEADER)pTempBuff;;
        pTempMsgHeader->pConn = pConn;
        pTempMsgHeader->iCurrsequence = pConn->iCurrSequence;
        //然后把包头往里面写
        pTempBuff += miLenMsgHeader;
        memcpy(pTempBuff, pPkgHeader, miLenPkgHeader);
        if(ePkgLen == miLenPkgHeader)
        {
            //表示该包没有包体
            //表示包已经收完整了
            //做一些后续处理就是
            // NgxLogStderr(0,"request::pkglen:%d",ePkgLen);
            if(1 == mFloodAttackEnable)
            {
                isFlood = TestFlood(pConn);
            }
            NgxWaitRequsetHandlerProcLast(pConn,isFlood);
        }
        else
        {
            //没有收完就开始收包体
            pConn->curStat = _PKG_BD_INIT;
            pConn->precvbuf = pTempBuff + miLenPkgHeader;
            pConn->irecvLen = ePkgLen - miLenPkgHeader;
        }

    }

    return;
}

//收到一个完整包的处理
void CSocket::NgxWaitRequsetHandlerProcLast(lpngxConnectionT pConn, bool &isFlood)
{
    // //收到完整的包就放入到消息队列中
    // int irmqc = 0;
    // InMsgRecvQueue(c->pneMemPointer,irmqc);

    // //激发线程池中的某个线程来处理业务逻辑
    // gThreadPool.Call(irmqc);

    if(false == isFlood)
    {
        gThreadPool.InMsgRecvQueueAndSignal(pConn->precvMemPointer);
    }
    else
    {
        CMemory * pMemory  = CMemory::GetInstance();
        pMemory->FreeMemory(pConn->precvMemPointer);
    }

    // gThreadPool.InMsgRecvQueueAndSignal(pConn->precvMemPointer);

    //放入消息队列中的包，就不需要内存池来管理收来的包了
    // pConn->ifnewrecvMem = false;
    pConn->precvMemPointer = nullptr;
    pConn->curStat = _PKG_HD_INIT;
    pConn->precvbuf = pConn->dataHeadInfo;
    pConn->irecvLen = miLenPkgHeader;
    return;
}

//发送数据专用函数,返回本次发送的字节数
//返回 > 0 ,成功发送了一些字节数
// = 0,估计对方断了
ssize_t CSocket::SendProc(lpngxConnectionT c, char * buff, ssize_t size)
{
    ssize_t n;
    for(;;)
    {
        n = send(c->fd, buff, size, 0);
        if(n > 0) //成功发送了一些数据
        {
            //发送成功一些数据,但发送了多少,
            //(1) n == size, 也就是想发送多少都发送成功了,这表示完全发完毕了
            //(2) n < size, 没法发送完毕,那肯定是发送缓冲区满了,所以也不必要重新发
            return n;
        }

        if(n == 0)
        {
            //send = 0 表示超时,对方主动关闭了连接过程.
            return 0;
        }

        if(errno == EAGAIN)
        {
            return -1;
        }

        if(errno == EINTR)
        {
            NgxLogStderr(errno, "CSocket::SendProc() 's send() failed");
        }
        else
        {
            return -2;
        }
    }
}

//设置发送时的处理函数
void CSocket::NgxWriteRequestHandler(lpngxConnectionT pConn)
{
    CMemory * pMeomry = CMemory::GetInstance();

    ssize_t sendSize = SendProc(pConn,pConn->pSendBuf,pConn->iSendLen);

    if(sendSize > 0 && sendSize != pConn->iSendLen)
    {
        //没有全部发送完毕
        pConn->pSendBuf = pConn->pSendBuf + sendSize;
        pConn->iSendLen = pConn->iSendLen - sendSize;
        return;
    }
    else if(-1 == sendSize)
    {
        NgxLogStderr(errno,"CSocket::NgxWriteRequestHandler magic error");
        return;
    }

    if(sendSize > 0 && sendSize == pConn->iSendLen)
    {
        //成功发送完数据之后，要把这个事件从红黑树中取消
        if(NgxEpollOperEvent(pConn->fd,
                             EPOLL_CTL_MOD,
                             EPOLLOUT,
                             1,
                             pConn) == -1)
        {
            NgxLogStderr(errno,"CSocket::NgxWriteRequestHandler() NgxEpollOperEvent() failed");
        }

        NgxLogStderr(0,"CSocket::NgxWriteRequestHandler() 's data sent over!");
    }


    //进行一些收尾工作
    if(sem_post(&mSemEventSendQueue) == -1)
    {
        NgxLogStderr(0,"CSocket::NgxWriteRequestHandler() semPost() failed");
    }

    pMeomry->FreeMemory(pConn->pSendMemPoint);
    pConn->pSendMemPoint = nullptr;
    --pConn->iThrowSendCount;       //这里要把顺序放出去
    return;

}


// //这里加入到消息队列中
// void CSocket::InMsgRecvQueue(char * buf, int & irmqc)
// {
//     CLock lock(&mRecvMessageQueueMutex);    
//     //因为这里就是写了析沟函数，所以在离开这个函数的时候，会自动解锁互斥量
//     mMsgRecvQueue.push_back(buf);
//     ++miRecvMsgQueueCount;
//     irmqc = miRecvMsgQueueCount;


//     //防止消息队列过大
//     // tempOutMsgRecvQueue();
//     // NgxLogStderr(0,"receive a entity data package, (header and body)");
// }



//从消息队列中把一个包提取出来以备后续处理
// char * CSocket::outMsgRecvQueue()
// {
//     CLock lock(&mRecvMessageQueueMutex);
//     if(mMsgRecvQueue.empty())
//     {
//         return nullptr;
//     }

//     char * sTempMsgBuf = mMsgRecvQueue.front();
//     mMsgRecvQueue.pop_front();
//     --miRecvMsgQueueCount;  //这里为什么不更新这个值
//     return sTempMsgBuf;
// }

//消息处理函数，专门处理各种收到的TCP消息
//消息是【消息头+包头+包体】
void CSocket::ThreadRecvProcFunc(char *pMsgBuf)
{
    return;
}

// void CSocket::tempOutMsgRecvQueue()
// {
//     if(mMsgRecvQueue.empty())
//     {
//         return;
//     }
//     int size = mMsgRecvQueue.size();
//     if(size < 1000)
//     {
//         //do nothing
//         return;
//     }
//     //超多了一定数量，开始清队列
//     CMemory * pMemory = CMemory::GetInstance();
//     int minus = size - 500;
//     for(int i = 0; i < minus; ++i)
//     {
//         char * sTempMsgBuf = mMsgRecvQueue.front();
//         mMsgRecvQueue.pop_front();          //
//         pMemory->FreeMemory(sTempMsgBuf);
//     }
//     return;
// }

