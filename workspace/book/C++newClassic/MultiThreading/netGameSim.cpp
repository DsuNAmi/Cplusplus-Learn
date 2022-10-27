/* 
模拟收发数据，
两个线程，一个用来接受发过来的数据，一个用来取出数据

对于一开始的初始程序，因为没有使用互斥量，会导致程序在读写的同时操作，导致程序异常，使用互斥量mutex来解决该问题



*/


#include <iostream>
#include <list>
#include <thread>
#include <mutex>
#include <condition_variable>

// class A
// {
//     private:
//         std::list<int> msgRecvQueue;
//         std::mutex myMutex;
//     public:
//         void inMsgRecvQueue()
//         {
//             for(int i = 0; i < 100000; ++i)
//             {
//                 std::cout << "InMsgRecvQueue()执行，插入一个元素" << i << std::endl;
//                 myMutex.lock();
//                 msgRecvQueue.push_back(i);
//                 myMutex.unlock();
//             }
//         }

//         void outMsgRecvQueue()
//         {
//             int command = 0;
//             for(int i = 0; i < 100000; ++i)
//             {
//                 bool result = outMsgLULProc(command);
//                 if(result)
//                 {
//                     std::cout << "outMsgRecvQueue()执行了，从容器取出了一个元素" << command << std::endl;
//                     //do somethings to deal with data;
//                 }
//                 else
//                 {
//                     std::cout << "outMsgRecvQueue()执行了，但是目前消息队列中是空元素" << std::endl;
//                 }
//             }
//             std::cout << "end" << std::endl;
//         }

//         bool outMsgLULProc(int & command)
//         {
//             myMutex.lock();
//             if(!msgRecvQueue.empty())
//             {
//                 command = msgRecvQueue.front();
//                 msgRecvQueue.pop_front();
//                 myMutex.unlock();
//                 return true;
//             }
//             myMutex.unlock();       //这里是两个出口，不然条件不满足就不会放锁了
//             return false;
//         }

//         // //std::lock_guard版本
//         // bool outMsgLULProc(int & command)
//         // {
//         //     std::lock_guard<std::mutex> sbgguard(myMutex);
//         //     // myMutex.lock();
//         //     if(!msgRecvQueue.empty())
//         //     {
//         //         command = msgRecvQueue.front();
//         //         msgRecvQueue.pop_front();
//         //         // myMutex.unlock();
//         //         return true;
//         //     }
//         //     // myMutex.unlock();       //这里是两个出口，不然条件不满足就不会放锁了
//         //     return false;
//         // }

// };

//这里写一个unique_lock的版本

/* 
std::try_to_lock
系统会尝试用mutex的lock去锁定这个mutex,但是如果没有锁成功，也会立即返回，而不是阻塞在那里
但是使用这个参数的前提条件是程序员不要自己去调用lock去锁这个mutex，不然两次锁就会死锁了。

std:defer_lock
前提是程序园不能自己先去lock这个mutex。
该参数的意思就是初始化这个mutex，但是这个选项表示并没有给这个mutex加锁，初始化一个没有加锁的mutex。
通过一个没有加锁的mutex，可以灵活地调用很多unique_lock相关的成员函数。
\

try_lock
成员函数，尝试去拿锁，拿到就返回true，否则false

release
成员函数，返回他所管理的mutex对象指针，并释放所有权。这就是unique_lock和mutex不再有关系，

*/


// class A
// {
//     private:
//         std::list<int> msgRecvQueue;
//         std::mutex myMutex;
//     public:
//         void inMsgRecvQueue()
//         {
//             for(int i = 0; i < 100000; ++i)
//             {
//                 std::cout << "InMsgRecvQueue()执行，插入一个元素" << i << std::endl;
//                 // std::unique_lock<std::mutex> sbguard1(myMutex, std::defer_lock);
//                 // if(sbguard1.owns_lock())
//                 // {
//                 //     //拿到了锁头，离开sbguard1作用域会自动释放
//                 //     msgRecvQueue.push_back(i);
//                 //     //其他处理代码
//                 // }
//                 // else
//                 // {
//                 //     //没拿到锁
//                 //     std::cout << "InMsgRecvQueue()执行，但是没有拿到锁，只能干点别的事" << i << std::endl;
//                 // }
//                 //lock函数
//                 // sbguard1.lock();    //加锁，无法加锁会阻塞
//                 // msgRecvQueue.push_back(i);
//                 //try_lock()函数
//                 // if(sbguard1.try_lock() == true)
//                 // {
//                 //     msgRecvQueue.push_back(i);
//                 // }
//                 // else
//                 // {
//                 //     std::cout << "没有拿到锁，做点别的事吧" << std::endl; 
//                 // }
//                 std::unique_lock<std::mutex> sbguard1(myMutex);
//                 std::mutex * pMutex = sbguard1.release();
//                 msgRecvQueue.push_back(i);
//                 pMutex->unlock();       //因为之前加锁，这里就要解锁，只不过是不能通过unique_lock去解锁了
//             }
//         }

//         void outMsgRecvQueue()
//         {
//             int command = 0;
//             for(int i = 0; i < 100000; ++i)
//             {
//                 bool result = outMsgLULProc(command);
//                 if(result)
//                 {
//                     std::cout << "outMsgRecvQueue()执行了，从容器取出了一个元素" << command << std::endl;
//                     //do somethings to deal with data;
//                 }
//                 else
//                 {
//                     std::cout << "outMsgRecvQueue()执行了，但是目前消息队列中是空元素" << std::endl;
//                 }
//             }
//             std::cout << "end" << std::endl;
//         }

//         bool outMsgLULProc(int & command)
//         {
//             std::unique_lock<std::mutex> sbguard1(myMutex);
//             std::chrono::milliseconds dura(200);
//             std::this_thread::sleep_for(dura);      //卡20000毫秒
//             if(!msgRecvQueue.empty())
//             {
//                 command = msgRecvQueue.front();
//                 msgRecvQueue.pop_front();
//                 return true;
//             }
//             return false;
//         }
// };

//条件变量
class A
{
    private:
        std::list<int> msgRecvQueue;
        std::mutex myMutex;
        std::condition_variable myCond;         //生成一个条件对象

    public:
        void InMsgRecvQueue()
        {
            for(int i = 0; i < 10000; ++i)
            {
                std::cout << "inMsgRecvQueue()执行，插入一个元素" << std::endl;
                std::unique_lock<std::mutex> sbguard1(myMutex);
                msgRecvQueue.push_back(i);
                myCond.notify_one();    //尝试把堵塞在wait()的线程唤醒，但仅仅唤醒了还不够，这里必须把互斥量解锁，另一个线程的wait()才会正常工作。
            }
            return;
        }

        bool OutMsgLULProc(int & command)
        {
            std::unique_lock<std::mutex> sbguard1(myMutex);
            if(!msgRecvQueue.empty())
            {
                command = msgRecvQueue.front();
                msgRecvQueue.pop_front();
                return true;
            }
            return false;
        }


        void OutMsgRecvQueue()
        {
            int command = 0;
            while(true)
            {
                std::unique_lock<std::mutex> sbguard1(myMutex);
                //wait()用于等待一个东西
                //如果wait()第二个参数lambda表达式返回的是true,wait就直接返回
                //如果wait()第二个参数lambda表达式返回的是false,那么wait()将解锁mutex并堵塞到这一行。堵到其他某个线程调用noify_one()通知为止.
                //如果wait()用不到第二个参数，那跟第二个返回false效果一样.
                myCond.wait(sbguard1,[this](){
                    if(!msgRecvQueue.empty())
                    {
                        return true;
                    }
                    return false;
                });
                //现在互斥量是被上锁的，流程走下来说明队列中必然有元素
                command = msgRecvQueue.front();
                msgRecvQueue.pop_front();
                sbguard1.unlock(); //
                std::cout << "outMsgRecvQueue()执行，取出一个元素" << command << std::endl;
            }
            //更新条件变量的写法
            // int command = 0;
            // for(int i = 0; i < 10000; ++i)
            // {
            //     bool result = OutMsgLULProc(command);
            //     if(result)
            //     {
            //         std::cout << "outMsgRecvQueue()执行了，从容器中取出一个元素" << std::endl;
            //     }
            //     else 
            //     {
            //         std::cout << "outMsgRecvQueue()执行了，但是队列是空队列" << std::endl;
            //     }
            // }
            // std::cout << "end" << std::endl;
        }
};


int main()
{
    A myobja;
    std::thread myOutnMsgObj(&A::OutMsgRecvQueue, &myobja);         //这里第二个参数必须是引用，因为要同一个对象，不能是复制的，或者使用std::ref
    std::thread myInMsgObj(&A::InMsgRecvQueue, &myobja);
    myInMsgObj.join();
    myOutnMsgObj.join();
    std::cout << "游戏信息收发完毕" << std::endl;
    return 0;
}
