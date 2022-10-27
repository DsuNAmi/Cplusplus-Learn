#include <iostream>
#include <thread>
#include <vector>
#include <mutex>

using namespace std;


// void myprint()
// {
//     cout << "我的线程开始执行了" << endl;
//     //
//     cout << "我的线程执行完毕了" << endl;

//     cout << "测试更多的输出" << endl;

//     return;
// }


// class TA
// {
//     public:
//         int & m_i;
//         TA(int & i) : m_i(i) {}
//         void operator()()
//         {
            
//             cout << "mi1的值为" << m_i << endl;     //可能m_i没有有效值
//             cout << "mi2的值为" << m_i << endl;
//             cout << "mi3的值为" << m_i << endl;
//             cout << "mi4的值为" << m_i << endl;
//             cout << "mi5的值为" << m_i << endl;
//             cout << "mi6的值为" << m_i << endl;
//             cout << "mi7的值为" << m_i << endl;
//             cout << "mi8的值为" << m_i << endl;
//         }
// };


// void myprint(const int & i, char * pmybuf)
// {
//     cout << i << endl;
//     cout << pmybuf << endl;
//     return;
// }

// void modiMyPrint(int i, const string & pmybuf)
// {
//     //这里会为const对象创建一个临时对象，即使是引用
//     cout << i << endl;
//     const char * ptmp = pmybuf.c_str();
//     cout << pmybuf.c_str() << endl;
//     return;
// }

// class A
// {
//     public:
//         A(int num) : m_i(num) {}
//         A(const A & a) {}
//         ~A() {}
//         void thread_work(int num)
//         {
//             cout << "subthread thread_work exec, this = " << this << " threaid = " << this_thread::get_id() << endl;
//         }
//     int m_i;
// };


// vector<int> gValue = {1,2,3};

// void myprint(int num)
// {
//     cout << "id is " << this_thread::get_id() << "'s print value is " << gValue[0] << gValue[1] << gValue[2] << endl;
// }


int main()
{
    // thread mytobj(myprint);     //创建一个线程，参数是一个函数名，这里会立即执行该函数
    // // mytobj.join();              //join会卡在这里，等待myprint线程执行完毕，程序流程结束才会往下走,注释掉会出异常
    // mytobj.detach();                //这里会使线程分离，但是主程序执行完了，子线程会被挂起来，
    // cout << "main程序执行结束" << endl;
    
    // thread mytobj(myprint);

    // if(mytobj.joinable())
    // {
    //     cout << "l : joinable() == true" << endl;           //chengli
    // }
    // else
    // {
    //     cout << "l : joinable() == false" << endl;
    // }

    // mytobj.join();

    // if(mytobj.joinable())
    // {
    //     cout << "l : joinable() == true" << endl;
    // }
    // else
    // {
    //     cout << "l : joinable() == false" << endl;       //chengli
    // }
    
    // int myi = 6;
    // TA ta(myi);
    // thread mytobj3(ta);
    // mytobj3.detach();
    // cout << "main函数执行完毕" << endl;



    // auto mylamthread = [](){
    //     cout << "我们的线程开始了" << endl;
    //     cout << "我们的线程完毕了" << endl;
    // };


    // thread mytobj(mylamthread);
    // mytobj.join();
    // cout << "main主函数执行结束" << endl;

    // int mvar = 1;
    // int & mvary = mvar;
    // char mybuf[] = "this is a best!";
    // thread mytobj(myprint, mvar, mybuf);
    // mytobj.join();

    // cout << "main函数执行完毕" << endl;


    // A myobj(10);
    // thread mytobj(&A::thread_work, myobj, 15);
    // mytobj.join();


    // vector<thread> mythreads;

    // for(int i = 0; i < 5; ++i)
    // {
    //     mythreads.push_back(thread(myprint, i));
    // }

    // for(auto iter = mythreads.begin(); iter != mythreads.end(); ++iter)
    // {
    //     iter->join();
    // }

    // cout << "main函数执行完毕" << endl;
    


    return 0;
}