#ifndef __NGX_THREADPOOL_H__
#define __NGX_THREADPOOL_H__

#include <vector>
#include <pthread.h>
#include <atomic> //c+11的原子操作
#include <list>


//线程池相关类
class CThreadPool
{
    public:
        CThreadPool();
        ~CThreadPool();

        bool Create(int threadNum);     //创建线程池中的所有线程
        void StopAll();                 //退出池子中的所有线程
        void Call();           //调用一个线程来干活

        void InMsgRecvQueueAndSignal(char * buf);
        //收到一个消息后，加入消息队列，并触发线程池来处理消息
        void ClearMsgRecvQueue();
        // int GetRecvMsgQueueCount();

    private:
        static void * ThreadFunc(void * threadData);
        //新线程的回调函数

        //定义一个线程池中的线程结构，以后可能做一些统计之类
        struct ThreadItem
        {
            pthread_t _Handle;          //线程句柄
            CThreadPool * _pThis;           //记录线程池的指针
            bool        ifrunning;          //标记是否正式启动起来


            ThreadItem(CThreadPool * pThis) : _pThis(pThis),ifrunning(false){}
            ~ThreadItem() {}

        };



        static pthread_mutex_t      mPthreadMutex;      //线程同步互斥量/也叫线程同步锁
        static pthread_cond_t       mPthreadCond;       //线程同步条件变量
        static bool                 mShutDown;          //线程退出标志，false不退出，true退出

        int                         miThreadNum;        //要创建的线程数量

        std::atomic<int>            miRunningThreadNum; //线程数，运行中的线程数，原子操作
        time_t                      miLastEmgTime;      //上次发生线程不够用【紧急事件】的时间，防止日志报的太频繁

        std::vector<ThreadItem *>   mThreadVector;      //线程放到容器里

        //接受消息队列
        std::list<char *>           mMsgRecvQueue;  //接受消息队列
        int                         mIRecvMsgQueueCount;    //收消息队列
};

#endif