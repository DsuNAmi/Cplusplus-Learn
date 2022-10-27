#include <iostream>
#include <thread>
#include <mutex>

std::mutex resourceMutex;
std::once_flag gFlag;

class MyCAS
{
    private:
        MyCAS() {}
    
    private:
        static MyCAS * m_instance;

    private:
        static void CreateInstance()
        {
            //如下两行是测试代码
            // std::chrono::milliseconds dura(2000);
            // std::this_thread::sleep_for(dura);
            m_instance = new MyCAS();
            std::cout << "CreateInstance()执行完毕" << std::endl;
            static CGarhuishou cl;
        }

    public:
        static MyCAS * GetInstance()
        {
            // if(m_instance == nullptr)
            // {   
            //     //双重锁定，或者说是双重检查
            //     std::unique_lock<std::mutex> myMutex(resourceMutex);
            //     if(m_instance == nullptr)
            //     {
            //         m_instance = new MyCAS();
            //         static CGarhuishou cl; //这里用一个静态成员是因为其生命周期是整个程序，所以要使用静态对象，这里就会在程序结束的时候才释放MyCAS创建的对象。

            //     }
            // }
            // return m_instance;

            //使用call_once的代码
            if(m_instance == nullptr)   //加这一句话能提高效率，这是因为后面创建了对象之后，就不需要判断是否还需要执行该函数了
            {
                std::call_once(gFlag, CreateInstance);
            }
            //两个线程同时执行到这里，其中一个线程卡在这行等另一个线程该行执行完毕。
            return m_instance;
        }
        class CGarhuishou
        {
            public:
                ~CGarhuishou()
                {
                    if(MyCAS::m_instance)
                    {
                        delete MyCAS::m_instance;
                        MyCAS::m_instance = nullptr;
                    }
                }
        };

        void func()
        {
            std::cout << "test" << std::endl;
        }
};

MyCAS * MyCAS::m_instance = nullptr;


void myThread()
{
    std::cout << "thread start" << std::endl;
    MyCAS * pa = MyCAS::GetInstance();
    std::cout << "thread end" << std::endl;
    return;
}


int main()
{
    // MyCAS * pa = MyCAS::GetInstance();
    // pa->func();
    // MyCAS::GetInstance()->func();

    // std::cout << "test" << std::endl;
    

    //如果没有加任何保护的前提下，这多线程会出现同时进行new 的情况，因为有可能两个线程同时判断m_instance为nullptr

    std::thread mytobj1(myThread);
    std::thread mytobj2(myThread);

    mytobj1.join();
    mytobj2.join();

    return 0;
}