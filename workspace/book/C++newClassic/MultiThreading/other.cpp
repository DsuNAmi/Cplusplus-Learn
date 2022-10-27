#include <iostream>
#include <thread>
#include <future>
#include <vector>

/* 
std::async,通常是用来启动一个异步任务，启动这个异步任务后，它会返回一个std::future;

-：参数
std::launch::deferred, 如果不等待futrure调用get或者是wait，这个线程是不会执行的

std::launch::async 调用时就创建线程执行

std::lanuch::deferred | std::launch::async 表示可能是两种中的一中


-----------------------------------------------------
std::packaged_task 这个是可以打包任务的，可以直接把函数作为参数，同时可以直接传一个lambda表达式，
这里可以直接调用这个task，也可以作为线程参数传递给线程

-----------------------------------------------------
std::promise
能够在某个线程中为其赋值，然后就可以在其他的线程中，把这个值取出来


*/



std::vector<std::packaged_task<int(int)>> myTasks;


int testCount = 0;
std::mutex testMutex;

std::atomic<int> atomicCount = 0;       //定义原子操作，不会被打断得



class A
{
    public:
        int myThread(int mypar)
        {
            std::cout << mypar << std::endl;
            std::cout << "mythread start" << "threadId = " << std::this_thread::get_id() << std::endl;
            std::chrono::milliseconds dura(20000);
            std::this_thread::sleep_for(dura);
            std::cout << "mythread end " << "threadId = " << std::this_thread::get_id() << std::endl;
            return 5;
        }
};


int MyThread()
{
    std::cout << "mythread start" << "threadId = " << std::this_thread::get_id() << std::endl;
    std::chrono::milliseconds dura(20000);
    std::this_thread::sleep_for(dura);
    std::cout << "mythread end " << "threadId = " << std::this_thread::get_id() << std::endl;
    return 5;
}


int PackThread(int mypar)
{
    std::cout << mypar << std::endl;
    std::cout << "mythread start" << "threadId = " << std::this_thread::get_id() << std::endl;
    std::chrono::milliseconds dura(5000);
    std::this_thread::sleep_for(dura);
    std::cout << "mythread end " << "threadId = " << std::this_thread::get_id() << std::endl;
    return 5;
}

void PromiseThread(std::promise<int> & tmpp, int calc)
{
    std::cout << "PromiseThread() start " << " ThreadId = " << std::this_thread::get_id() << std::endl;
    // do somethings
    ++calc;
    calc *= 10;

    //模拟其他操作，5秒
    std::chrono::milliseconds dura(5000);
    std::this_thread::sleep_for(dura);

    int result = calc;
    tmpp.set_value(result);         //结果保存到tmpp中

    std::cout << "PromiseThread() end" << " ThreadId = " << std::this_thread::get_id() << std::endl; 
}

void AddThread()
{
    for(int i = 0; i < 100000000; ++i)
    {
        // testMutex.lock();
        // testCount++;
        // testMutex.unlock();
        // // ++testCount;
        atomicCount++;
    }

    return;
}

int main()
{
    //普通函数
    // std::cout << "main " << "threadId = " << std::this_thread::get_id() << std::endl;
    // std::future<int> result = std::async(MyThread);

    // std::cout << "continue..." << std::endl;
    // std::cout << result.get() << std::endl;     //等待线程执行完毕，必须卡在这里等待这个线程执行完毕才行。

    // std::cout << "main主函数执行结束" << std::endl;


    //类成员函数
    // A a;
    // int tmppar = 12;
    // std::cout << "main " << "threadId = " << std::this_thread::get_id() << std::endl;
    // // std::future<int> result = std::async(&A::myThread, &a, tmppar);      //和std::launch::deferred | std::launch::async的效果一样
    // // std::future<int> result = std::async(std::launch::deferred, &A::myThread, &a, tmppar);
    // std::future<int> result = std::async(std::launch::async, &A::myThread, &a, tmppar);
    // // std::future<int> result = std::async(std::launch::deferred | std::launch::async, &A::myThread, &a, tmppar);

    // std::cout << "continue..." << std::endl;
    // std::cout << result.get() << std::endl;     //等待线程执行完毕，必须卡在这里等待这个线程执行完毕才行。

    // std::cout << "main主函数执行结束" << std::endl;


    //看一下std::packaged_task的用法
    // std::cout << "main " << "threadId = " << std::this_thread::get_id() << std::endl;
    // std::packaged_task<int(int)> mypt(PackThread);

    // std::thread t1(std::ref(mypt), 1);
    // t1.join();          //不调用程序会崩溃
    // std::future<int> result = mypt.get_future();        //返回PackThread的结果
    // std::cout << result.get() << std::endl;

    // std::cout << "main函数执行完毕" << std::endl;



    // std::cout << "main " << " ThreadId = " << std::this_thread::get_id() << std::endl;
    // std::packaged_task<int(int)> mypt([](int mypar){
    //     std::cout << mypar << std::endl;
    //     std::cout << "lambda myThread() start " << " ThreadId = " << std::this_thread::get_id() << std::endl;
    //     std::chrono::milliseconds dura(5000);
    //     std::this_thread::sleep_for(dura);
    //     std::cout << "lambda myThread() end" << " ThreadId = " << std::this_thread::get_id() << std::endl;
    //     return 15;
    // });

    // myTasks.push_back(std::move(mypt));         //注意，这里move掉之后，mypt就没有所用的对象
    
    // std::packaged_task<int(int)> mypt2;
    // auto iter = myTasks.begin();
    // mypt2 = std::move(*iter);
    // myTasks.erase(iter);        //删除第一个元素，因为这个迭代器没有带东西了，被move了
    // mypt2(123);
    // std::future<int> result = mypt2.get_future();
    // std::cout << result.get() << std::endl;

    // std::cout << "main函数执行完毕" << std::endl;

    //测试std::promise
    // std::cout << "main " << " ThreadId = " << std::this_thread::get_id() << std::endl;
    // std::promise<int> myProm;
    // std::thread t1(PromiseThread, std::ref(myProm), 180);
    // t1.join();

    // std::future<int> fu1 = myProm.get_future();
    // auto result = fu1.get();

    // std::cout << "result = " << result << std::endl;

    // std::cout << "main 函数执行完毕" << std::endl;  

    //测试++的次数
    std::thread t1(AddThread);
    std::thread t2(AddThread);
    t1.join();
    t2.join();

    std::cout << atomicCount << std::endl;


    return 0;
}