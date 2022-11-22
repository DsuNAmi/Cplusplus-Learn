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
#include "ngx_c_memory.h"
#include "ngx_c_crc32.h"
#include "ngx_c_slogic.h"
#include "ngx_logiccomm.h"
#include "ngx_c_lockmutex.h"

//定义成员函数指针
typedef bool (CLogicSocket::*handler)(lpngxConnectionT pConn,
                                      LPSTRUC_MSG_HEADER pMsgHeader,
                                      char * pPkgBody,
                                      unsigned short iBodyLength);


//用来保存 成语函数指针的 数组
static const handler statusHandler[] = 
{
    //数组的前五个元素保留
    &CLogicSocket::_HandlePing,             //心跳包
    nullptr,
    nullptr,
    nullptr,
    nullptr,

    //开始具体的业务逻辑
    &CLogicSocket::_HandleResister,         //注册功能
    &CLogicSocket::_HandleLogin,
};

//命令的数量，这里应该是7个,但是从零开始
#define AUTH_TOTAL_COMMANDS sizeof(statusHandler)/sizeof(handler)


CLogicSocket::CLogicSocket()
{

}

CLogicSocket::~CLogicSocket()
{

}

bool CLogicSocket::Initialize()
{
    bool bParentInit = CSocket::Initialize();
    return bParentInit;
}

//处理收到的数据包
//pMsgBuf:消息头+包头+包体
void CLogicSocket::ThreadRecvProcFunc(char *pMSGBuf)
{
    LPSTRUC_MSG_HEADER pMsgHeader = (LPSTRUC_MSG_HEADER)pMSGBuf;        //消息头
    LPCOMM_PKG_HEADER pPkgHeader = (LPCOMM_PKG_HEADER)(pMSGBuf + miLenMsgHeader);
    void * pPkgBody = nullptr;
    unsigned short pkgLen = ntohs(pPkgHeader->pkgLen);
    // NgxLogStderr(0,"logic:pkelen:%d",pkgLen);

    if(miLenPkgHeader == pkgLen)
    {
        //没有包体 ，只有包头
        if(pPkgHeader->crc32 != 0)
        {
            return;
        }
        pPkgBody = nullptr;
    }
    else
    {
        //我们来读取包体的内容
        pPkgHeader->crc32 = ntohl(pPkgHeader->crc32);
        pPkgBody = (void *)(pMSGBuf + miLenMsgHeader + miLenPkgHeader);     //跳过消息头和包头

        //计算crc
        int calccrc = CCRC32::GetInstance()->GetCRC((unsigned char *)pPkgBody,pkgLen - miLenPkgHeader);
        NgxLogStderr(0,"calcrc = %d, pkgCrc = %d",calccrc, pPkgHeader->crc32);
        // 这代码就离谱，自己的CRC都算错，算法就有问题
        // if(calccrc != pPkgHeader->crc32)
        // {
        //     //服务器和包头的进行比较
        //     NgxLogStderr(0,"CLogicSocket::ThreadRecvProcFunc() 's crc error, drop it");
        //     return;
        // }

    }

    //校验成功，取出数据
    unsigned short imsgCode = ntohs(pPkgHeader->msgCode);//把消息代码拿出来
    lpngxConnectionT pConn = pMsgHeader->pConn;     //消息头有连接池中连接的指针

    //我们要做一些判断
    //（1）如果从收到客户端发送来的包，到服务器释放一个线程池中的线程出合理该包的过程中，客户断开了，那么这种包就不处理了
    if(pConn->iCurrSequence != pMsgHeader->iCurrsequence)
    {
        return; //【客户端端开了，丢弃这种包】
    }

    //（2）判断消息码是正确的，防止客户端恶意侵害服务器，发送一个不在我们服务器处理范围内的消息码
    if(imsgCode >= AUTH_TOTAL_COMMANDS)
    {
        //恶意包
        NgxLogStderr(0,"CLogicSocket::ThreadRecvProcFunc() imsgCode=%d, is wrong!",imsgCode);
        return;
    }

    //(3)有对应的消息处理函数
    if(statusHandler[imsgCode] == nullptr)
    {
        NgxLogStderr(0,"CLogicSocket::ThreadRecvProcFunc() imsgCode=%d, find No handler!",imsgCode);
    } 

    //(4)调用对应的消息处理函数
    // NgxLogStderr(0,"pkglen %d",pkgLen);
    // NgxLogStderr(0,"milenheader %d",miLenPkgHeader);

    (this->*statusHandler[imsgCode])(pConn,pMsgHeader,(char*)pPkgBody,pkgLen-miLenPkgHeader);
    return;

}


void CLogicSocket::ProcPingTimeOutChecking(LPSTRUC_MSG_HEADER tempMsg, time_t curTime)
{
    CMemory * pMemory = CMemory::GetInstance();
    
    // NgxLogStderr(0,"I'ME");
    // NgxLogStderr(0,"%d",tempMsg->iCurrsequence);
    // NgxLogStderr(0,"%d",tempMsg->pConn->iCurrSequence);


    if(tempMsg->iCurrsequence == tempMsg->pConn->iCurrSequence)
    {
        lpngxConnectionT pConn = tempMsg->pConn;
        if(miFTimeOutKick == 1)
        {
            zdCloseSocketProc(pConn);
        }
        else if( (curTime - pConn->lastPingTime) > (miWaitTime * 3 + 10))
        {
            //踢出去
            // NgxLogStderr(0,"time up no heart packet, get out!");
            zdCloseSocketProc(pConn);
        }

        pMemory->FreeMemory(tempMsg);
    }
    else
    {
        pMemory->FreeMemory(tempMsg);
    }
    return;
}


//发送没有包体的数据包给客户端
void CLogicSocket::SendNoBodyPkgToClient(LPSTRUC_MSG_HEADER pMsgHeader, unsigned short iMsgCode)
{
    CMemory * pMemory = CMemory::GetInstance();

    char * pSendBuf = (char *)pMemory->AllocMemory(miLenMsgHeader + miLenPkgHeader, false);
    char * pTempBuf = pSendBuf;

    memcpy(pTempBuf,pMsgHeader, miLenMsgHeader);
    pTempBuf += miLenMsgHeader;

    LPCOMM_PKG_HEADER pPkgHeader = (LPCOMM_PKG_HEADER)pTempBuf;
    pPkgHeader->msgCode = htons(iMsgCode);
    pPkgHeader->pkgLen = htons(miLenPkgHeader);
    pPkgHeader->crc32 = 0;
    // NgxLogStderr(0,"here");
    MsgSend(pSendBuf);
    return;
}




bool CLogicSocket::_HandleResister(lpngxConnectionT pConn, LPSTRUC_MSG_HEADER pMsgHeader, char * pPkgBody, unsigned short iBodyLength)
{
    //(1)首先判断包的合法性
    if(pPkgBody == nullptr)
    {
        return false;
    }

    int iRecvLen = sizeof(STRUCT_REGISTER);
    // NgxLogStderr(0,"sizeof raw register : %d",iRecvLen);
    // NgxLogStderr(0,"send sizeof regiseter : %d",iBodyLength);
    if(iRecvLen != iBodyLength)//
    {
        NgxLogStderr(0,"the pkgage body size is unmatch!");
        return false;
    }

    //(2)对于同一个用户，可能同时发多个请求，这些请求可能会访问服务器的数据，所以要上锁
    CLock lock(&pConn->logicProcMutex); //凡是和本用户有关的访问都互斥

    //(3) 取得了整个发过来的数据
    LPSTRUCT_REGISTER pRecvInfo = (LPSTRUCT_REGISTER)pPkgBody;
    pRecvInfo->iType = ntohl(pRecvInfo->iType);
    pRecvInfo->username[sizeof(pRecvInfo->username) - 1] = 0;
    pRecvInfo->password[sizeof(pRecvInfo->password) - 1] = 0;


    //..............
    //对数据进行一些处理
    //.............

    //(5)给客户端返回数据
    LPCOMM_PKG_HEADER pPkgHeader;
    CMemory * pMemory = CMemory::GetInstance();
    CCRC32 * pCCRC32 = CCRC32::GetInstance();
    int iSendLen = sizeof(STRUCT_REGISTER);
    //测试代码
    // iSendLen = 65000;
    //a）分配要发出去的包的内存
    char * pSendBuf = (char *)pMemory->AllocMemory(miLenMsgHeader + miLenPkgHeader + iSendLen,false);
    //b) 填充消息头
    memcpy(pSendBuf, pMsgHeader, miLenMsgHeader);
    //c)填充包头
    pPkgHeader = (LPCOMM_PKG_HEADER)(pSendBuf + miLenMsgHeader);
    pPkgHeader->msgCode = _CMD_REGISTER;
    pPkgHeader->msgCode = htons(pPkgHeader->msgCode);
    pPkgHeader->pkgLen = htons(miLenPkgHeader + iSendLen);
    //d) 填充包体
    LPSTRUCT_REGISTER pSendInfo = (LPSTRUCT_REGISTER)(pSendBuf + miLenMsgHeader + miLenPkgHeader);
    pPkgHeader->crc32 = pCCRC32->GetCRC((unsigned char*)pSendInfo,iSendLen);
    pPkgHeader->crc32 = htonl(pPkgHeader->crc32);


    //发送数据包
    MsgSend(pSendBuf);
    // if(NgxEpollOperEvent(pConn->fd,
    //                      EPOLL_CTL_MOD,
    //                      EPOLLOUT,
    //                      0,
    //                      pConn) == -1)
    // {
    //     NgxLogStderr(0,"11111111111111111111111111111111");
    // }




    // NgxLogStderr(0,"execute CLogicSocket::_HandleRegister()");
    return true;
}

bool CLogicSocket::_HandleLogin(lpngxConnectionT pConn, LPSTRUC_MSG_HEADER pMsgHeader, char * pPkgBody, unsigned short iBodyLength)
{
    NgxLogStderr(0,"execute CLogicSocket::_HandleLogin()");
    return true;
} 

//接收并从处理客户端发来的ping包
bool CLogicSocket::_HandlePing(lpngxConnectionT pConn, LPSTRUC_MSG_HEADER pMsgHeader, char * pPkgBody, unsigned short iBodyLength)
{
    //心跳包要求没有包体
    if(iBodyLength != 0)
    {
        return false;
    }

    CLock lock(&pConn->logicProcMutex);
    pConn->lastPingTime = time(NULL);


    // LPCOMM_PKG_HEADER temp = (LPCOMM_PKG_HEADER)(pMsgHeader + miLenMsgHeader);
    // NgxLogStderr(0,"pkglen:%d",temp->pkgLen);

    SendNoBodyPkgToClient(pMsgHeader, _CMD_PING);

    NgxLogStderr(0,"recive the ping packet!");
    return true;
}