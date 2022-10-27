#include <iostream>
#include <thread>
#include <windows.h>
#include <mutex>
#include <list>


#define __WINDOWSCS__

class A
{
    private:
        std::list<int> msgRecvQueue;
        std::mutex myMutex;
        #ifdef __WINDOWSCS__
            CRITICAL_SECTION myWinsec;
        #endif

    public:
        A()
        {
            #ifdef __WINDOWSCS__
                InitializeCriticalSection(&myWinsec);
            #endif
        }
        virtual ~A()
        {
            #ifdef __WINDOWSCS__
                DeleteCriticalSection(&myWinsec);
            #endif
        }

        void InMsgRecvQueue()
        {
            for(int i = 0; i < 100000; ++i)
            {
                std::cout << "InMsgRecvQueue()执行，插入一个元素" << i << std::endl;
                #ifdef __WINDOWSCS__
                    EnterCriticalSection(&myWinsec);
                    msgRecvQueue.push_back(i);
                    LeaveCriticalSection(&myWinsec);
                #else
                    myMutex.lock();
                    msgRecvQueue.push_back(i);
                    myMutex.unlock();
                #endif
            }
        }

        bool OutMsgLULProc(int & command)
        {
            #ifdef __WINDOWSCS__
                EnterCriticalSection(&myWinsec);
                if(!msgRecvQueue.empty())
                {
                    int command = msgRecvQueue.front();
                    msgRecvQueue.pop_front();
                    LeaveCriticalSection(&myWinsec);
                    return true;
                }
                LeaveCriticalSection(&myWinsec);
            #else
                myMutex.lock();
                if(!msgRecvQueue.empty())
                {
                    int command = msgRecvQueue.front();
                    msgRecvQueue.pop_front();
                    myMutex.unlock();
                    return true;
                }
                myMutex.unlock();
            #endif
            return false;
        }

        void OutMsgRecvQueue()
        {
            int command = 0;
            for(int i = 0; i < 100000; ++i)
            {
                bool result = OutMsgLULProc(command);
                if(result)
                {
                    std::cout << "outMsgRecvQueue()执行了，取出了一个元素" << command << std::endl;
                }
                else
                {
                    std::cout << "outMsgRecvQueue()执行了，队列为空" << std::endl;
                }
            }
            std::cout << "end" << std::endl;
        }

};



int main()
{
    return 0;
}