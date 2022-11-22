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



//设置踢出时间，用户三次握手成功连入，然后我们开启了体人开关
void CSocket::AddToTimeQueue(lpngxConnectionT pConn)
{
    CMemory * pMemory = CMemory::GetInstance();

    time_t futTime = time(NULL);
    futTime += miWaitTime;
    CLock lock(&mTimeQueueMutex);
    LPSTRUC_MSG_HEADER tempMsgHeader = (LPSTRUC_MSG_HEADER)pMemory->AllocMemory(miLenMsgHeader,false);
    tempMsgHeader->pConn = pConn;
    tempMsgHeader->iCurrsequence = pConn->iCurrSequence;
    mTimeQueueMap.insert(std::make_pair(futTime,tempMsgHeader));        //按照升序排列
    ++mCurSize;
    mTimerValue = GetEarliestTime();
    return;
}


//从map中获取最早的时间返回，调用函数是互斥的，所以这个函数就不用互斥了
time_t CSocket::GetEarliestTime()
{
    std::multimap<time_t, LPSTRUC_MSG_HEADER>::iterator pos;
    pos = mTimeQueueMap.begin();
    return pos->first;
}

//移除超时节点并返回
LPSTRUC_MSG_HEADER CSocket::RemovFirstTimer()
{
    std::multimap<time_t, LPSTRUC_MSG_HEADER>::iterator pos;
    LPSTRUC_MSG_HEADER pTemp;
    if(mCurSize <= 0)
    {
        return nullptr;
    }
    pos = mTimeQueueMap.begin();
    pTemp = pos->second;
    mTimeQueueMap.erase(pos);
    --mCurSize;
    return pTemp;
}


//根据当前给的时间，从mTimeQueue里找到比这个时间更早的节点返回去
LPSTRUC_MSG_HEADER CSocket::GetOverTimeTimer(time_t curTime)
{
    CMemory * pMemory = CMemory::GetInstance();
    LPSTRUC_MSG_HEADER pTemp;

    if(0 == mCurSize || mTimeQueueMap.empty())
    {
        return nullptr;
        //队列为空
    }

    time_t earliestTime = GetEarliestTime();        //寻找
    if(earliestTime <= curTime)
    {
        //[超时节点]
        pTemp = RemovFirstTimer();

        if(miFTimeOutKick != 1)
        {
            //因为下次超时也要判断，所以要把这个节点加回来
            time_t newInQueueTime = curTime + (miWaitTime);
            LPSTRUC_MSG_HEADER tempMsgHeader = (LPSTRUC_MSG_HEADER)pMemory->AllocMemory(sizeof(STRUC_MSG_HEADER),false);
            tempMsgHeader->pConn = pTemp->pConn;
            tempMsgHeader->iCurrsequence = pTemp->iCurrsequence;
            mTimeQueueMap.insert(std::make_pair(newInQueueTime, tempMsgHeader));
            ++mCurSize;
        }

        if(mCurSize > 0)
        {
            mTimerValue = GetEarliestTime();
        }
        return pTemp;
    }

    return nullptr;
}


//把指定用户TCP从timer表中踢出
void CSocket::DeteleFromTimeQueue(lpngxConnectionT pConn)
{
    std::multimap<time_t, LPSTRUC_MSG_HEADER>::iterator pos, posend;
    CMemory * pMemory = CMemory::GetInstance();

    CLock lock(&mTimeQueueMutex);

lblMTQM:
    pos = mTimeQueueMap.begin();
    posend = mTimeQueueMap.end();
    for(; pos != posend; ++pos)
    {
        pMemory->FreeMemory(pos->second);
        mTimeQueueMap.erase(pos);
        --mCurSize;
        goto lblMTQM;
    }

    if(mCurSize > 0)
    {
        mTimerValue = GetEarliestTime();
    }
    return;
}


void CSocket::ClearAllFromTimerQueue()
{
    std::multimap<time_t,LPSTRUC_MSG_HEADER>::iterator pos, posend;

    CMemory * pMeomry = CMemory::GetInstance();
    pos = mTimeQueueMap.begin();
    posend = mTimeQueueMap.end();
    for(;pos != posend;++pos)
    {
        pMeomry->FreeMemory(pos->second);
        --mCurSize;
    }
    mTimeQueueMap.clear();
}


//时间队列监视和处理函数，处理到期不发心跳包的用户踢出线程
void *CSocket::ServerTimeQueueMonitorThread(void * threadData)
{
    ThreadItem * pThread = static_cast<ThreadItem*>(threadData);
    CSocket * pSocketObj = pThread->_pThis;

    time_t  abosulteTime;
    time_t  curTime;

    int err;

    while(0 == gStopEvent)
    {
        if(pSocketObj->mCurSize > 0)
        {
            //时间队列中最近发生的事
            abosulteTime = pSocketObj->mTimerValue;
            curTime = time(NULL);
            if(abosulteTime < curTime)
            {
                //时间到了，就开始处理
                std::list<LPSTRUC_MSG_HEADER> mlsIdleLst;
                LPSTRUC_MSG_HEADER result;

                err = pthread_mutex_lock(&pSocketObj->mTimeQueueMutex);
                if(0 != err)
                {
                    NgxLogStderr(err,"CSocket::ServerTimerQueueMOnitorThread's pthread_mutex_lock() failed");
                }

                while((result = pSocketObj->GetOverTimeTimer(curTime)) != nullptr)  //一次性把超时节点都拿过来
                {
                    mlsIdleLst.push_back(result);
                }
                err = pthread_mutex_unlock(&pSocketObj->mTimeQueueMutex);
                if(0 !=  err)
                {
                    NgxLogStderr(err,"CSocket::ServerTimerQueueMonitorThread() pthread_mutex_unlock() failed");
                }
                LPSTRUC_MSG_HEADER tempMsg;
                while(!mlsIdleLst.empty())
                {
                    tempMsg = mlsIdleLst.front();
                    mlsIdleLst.pop_front();
                    pSocketObj->ProcPingTimeOutChecking(tempMsg,curTime);
                }

            }
        }

        //每次循环休息500毫秒
        usleep(500 * 1000);
    }

    return (void*)0;
}

void CSocket::ProcPingTimeOutChecking(LPSTRUC_MSG_HEADER tempMsg, time_t cutTime)
{
    NgxLogStderr(0,"hre!");
    CMemory * pMemory = CMemory::GetInstance();
    pMemory->FreeMemory(tempMsg);
}