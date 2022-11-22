#include <stdarg.h>
#include <unistd.h>


#include "ngx_global.h"
#include "ngx_func.h"
#include "ngx_c_threadpool.h"
#include "ngx_c_memory.h"
#include "ngx_macro.h"


//静态成员初始化
pthread_mutex_t CThreadPool::mPthreadMutex = PTHREAD_MUTEX_INITIALIZER;
//#define PTHREAD_MUTEX_INITIALIZER ((pthread_mutex_t) -1)
pthread_cond_t CThreadPool::mPthreadCond = PTHREAD_COND_INITIALIZER;
//#define PTHREAD_COND_INITIALIZER ((pthread_cond_t) -1)
bool CThreadPool::mShutDown = false;    //刚开始标记整个线程池是不退出的

CThreadPool::CThreadPool()
{
    miRunningThreadNum = 0;     //正在运行的线程，开始给0
    miLastEmgTime = 0;          //上次报告线程不够用了的时间  

    mIRecvMsgQueueCount = 0;
}


CThreadPool::~CThreadPool()
{
    //资源释放在StopAll()里统一进行
    ClearMsgRecvQueue();
}

//各种清理函数
//清理接受消息队列
void CThreadPool::ClearMsgRecvQueue()
{
    char * sTempMemPoint;
    CMemory * pMemory = CMemory::GetInstance();

    //尾声阶段，需要互斥？
    while(!mMsgRecvQueue.empty())
    {
        sTempMemPoint = mMsgRecvQueue.front();
        mMsgRecvQueue.pop_front();
        pMemory->FreeMemory(sTempMemPoint);
    }
}

//创建线程中的线程，要手工调用，不在构造函数里调用了
//return value，所有线程创建成功返回ture,出错误返回false
bool CThreadPool::Create(int threadNum)
{
    ThreadItem * pNew;      //首先定一个线程对象
    int err;

    miThreadNum = threadNum; //保存要创建线程的数量

    for(int i = 0; i < miThreadNum; ++i)
    {
        mThreadVector.push_back(pNew = new ThreadItem(this));
        err = pthread_create(&pNew->_Handle, nullptr, ThreadFunc, pNew);
        if(0 != err)
        {
            //创建线程失败
            NgxLogStderr(err, "CTheadPool::Create() create thread %d falied, return error value is %d",i, err);
            return false;
        }
        else
        {
            //创建成功，那就不做什么，也可以记录日志来标记谁做错了
        }
    }

    //我们必须保证每个线程都启动并运行到pthread_cond_wait(),本函数返回，只有这样，这几个线程才能进行后续的工作
    std::vector<ThreadItem*>::iterator iter;
lblfor:
    for(iter = mThreadVector.begin(); iter != mThreadVector.end(); ++iter)
    {
        if((*iter)->ifrunning == false) //这个条件保证所有线程启动起来，以保证整个线程池中的线程正常工作
        {
            //进到这里表示有没有完全启动的线程
            usleep(100 * 1000); //
            goto lblfor;
        }
    }
    return true;

}

//线程函数入口，当用pthread_create()创建线程后，这个ThreadFunc()会被立即执行
void * CThreadPool::ThreadFunc(void * threadData)
{
    //这个是静态成员函数，是不存在this指针的
    ThreadItem * pThread = static_cast<ThreadItem*>(threadData);
    //这个代码就写的很无语，这里又用强制转换
    CThreadPool * pThreadPoolObj = pThread->_pThis;

    CMemory * pMemory = CMemory::GetInstance();

    int err;

    pthread_t tid = pthread_self();     //获取线程自身id，以方便调试打印信息等

    while(true)
    {
        //锁一下这个线程的互斥量
        err = pthread_mutex_lock(&mPthreadMutex);
        if(0 != err)
        {
            NgxLogStderr(err,"CThreadPool::ThreadFunc() pthread_mutex_lock() failed, return error value is %d",err);
        }

        //注意这种写法
        while((pThreadPoolObj->mMsgRecvQueue.size() == 0) && mShutDown == false)
        {
            //如果这个pthread_cond_wait被唤醒，【被唤醒后程序执行流程往下走的前提是拿到了锁】
            //pthread_cond_wait()返回时，互斥量再次被锁住
            //那么会立即执行outMsgRecvQueue()如果拿到了一个nullptr,则继续在这里wait

            if(pThread->ifrunning == false)
            {
                pThread->ifrunning = true;
                //标记了true了才允许调用StopAll(),否则会线程混乱
            }


            //刚开始执行pthread_cond_wait()，会卡在这里，而且mPthreadMutex会被释放掉
            //这里就是让线程开启一个等待状态，因为既没有拿到队列中的数据，线程也不是关闭状态
            pthread_cond_wait(&mPthreadCond, &mPthreadMutex);

        }

        //能走下来的必然是拿到了真正消息队列中的数据，或者是mShutDown ==  true;
        //跳出循环要注意还是处于锁状态，因为这个时候不会有别的线程往里面加数据
        if(mShutDown)
        {
            pthread_mutex_unlock(&mPthreadMutex);
            break;
        }

        //这里就是因为之前把那个取出队列消息的给换了判断，所以就是这里必须要把消息取走
        char * jobBuf = pThreadPoolObj->mMsgRecvQueue.front();
        pThreadPoolObj->mMsgRecvQueue.pop_front();
        --pThreadPoolObj->mIRecvMsgQueueCount;

        //然后再解锁互斥量
        err = pthread_mutex_unlock(&mPthreadMutex);
        if(0 != err) 
        {
            NgxLogStderr(err,"CTheadPool::ThreadFunc() pthread_cond_wait() failed");
        }

        //能走到这里的就是有数据处理
        ++pThreadPoolObj->miRunningThreadNum;       //正在运行的数量加一，这是一个原子变量，所以不用加互斥量

        //对提出的消息进行处理
        gSocket.ThreadRecvProcFunc(jobBuf);

        // NgxLogStderr(0,"execute begin----------tid=%ui",tid);
        // sleep(5);//测试代码
        // NgxLogStderr(0,"execute end------------tid=%ui",tid);

        pMemory->FreeMemory(jobBuf);
        --pThreadPoolObj->miRunningThreadNum;       //执行完了就-1嘛


    }//这里处理改变mShutdown的状态，没有别的办法走出这个循环阿

    return (void*)0;
}


//停止所有线程【等待结束线程在池中所有线程，该函数返回后，应该是所有线程池中线程都结束了】
void CThreadPool::StopAll()
{
    //(1)已经关了就不管了
    if(mShutDown == true)
    {
        return;
    }
    mShutDown = true;

    //(2)唤醒等待该条件，
    int err = pthread_cond_broadcast(&mPthreadCond);
    //该函数用于将所有处理等状态的线程所等待的条件mPthreadCond设置成激发状态
    //以激发所有线程池中的线程，然后让线程的执行流程从卡住的地方走下去。
    if(0 != err)
    {
        NgxLogStderr(err,"CThreadPool::StopAll() 's pthread_cond_broadcase() failed return error value is %d",err);
        return;
    }


    //(3)等待线程，让线程真返回
    std::vector<ThreadItem*>::iterator iter;
    for(iter = mThreadVector.begin(); iter != mThreadVector.end(); ++iter)
    {
        pthread_join((*iter)->_Handle, nullptr);        //等待一个线程终止
        //这里就是因为有的线程没有拿到锁
        //退出的速度不一样，所以让线程都等在这里，直到所有的线程停止。
    }

    //流程走到这里，那么肯定所有的线程都停止了
    pthread_mutex_destroy(&mPthreadMutex);
    pthread_cond_destroy(&mPthreadCond);

    //(4)释放一下new出来的ThreadItem
    for(iter = mThreadVector.begin(); iter != mThreadVector.end(); ++iter)
    {
        if(*iter)
        {
            delete *iter;
        }
    }
    mThreadVector.clear();

    NgxLogStderr(0,"CThreadPool::StopAll() return successed, threadpoll end reagular!");
    return;
}


//收到一个完整消息后，入消息队列，并触发线程池中来处理消息
void CThreadPool::InMsgRecvQueueAndSignal(char * buf)
{
    //互斥
    int err = pthread_mutex_lock(&mPthreadMutex);
    if(0 != err)
    {
        NgxLogStderr(err,"CThreadPool::inMsgRecvQueueAndignal() pthread_mutex_lock() failed");
    }

    mMsgRecvQueue.push_back(buf);
    ++mIRecvMsgQueueCount;      //锁了所以不用原子操作

    //取消互斥
    err = pthread_mutex_unlock(&mPthreadMutex);
    if(0 != err)
    {
        NgxLogStderr(err,"CThreadPool::inMsgRecvQueueAndignal() pthread_mutex_unlock() failed");

    }   

    Call(); //这时候就直接调用来干活了
    return;
}


//来任务了，就调用一个线程下来干活
void CThreadPool::Call()
{
    int err = pthread_cond_signal(&mPthreadCond);
    //这个函数可以至少激发一个线程，激发有两种情况
    //1
    //线程常规情况下是卡在pthread_cond_wait，但是pthread_cond_signal
    //就能激发这个线程执行流程走下来。下来是一个while循环，这里第一个取出队列
    //元素的判断条件肯定不成功，所以就跳出循环，执行后面的逻辑
    //唤醒一个等待该条件的线程
    if(0 != err)
    {
        NgxLogStderr(err,"CThreadPool::Call()'s thread_cond_signal() failed return error value is %d",err);
    }

    //如果线程全部忙，那就要报警
    if(miThreadNum == miRunningThreadNum)
    {
        //线程不够用了
        time_t curTime = time(NULL);
        if(curTime - miLastEmgTime > 10) //每间隔10秒报一次警
        {
            miLastEmgTime = curTime;
            NgxLogStderr(0,"CTheadPool::Call*() have no thread, ");
        }
    }

    return;
}
