#include <iostream>

// #define isNs1 1
#define isGlobal 2

using namespace std;

void func() {cout << "::func()" << endl;}

namespace ns1
{
    void func()
    {
        cout << "ns1::func()" << endl;
    }
} // namespace ns1

namespace ns2
{
    #ifdef isNs1
        using ns1::func; //ns1中的函数
    #elif isGlobal
        using ::func; //全局中的func
    #else
        void func() {cout << "other::func()" << endl;}
    #endif
} // namespace ns2

int main()
{
    /* 
    这就是为什么c++使用了cmath而不是math.h头文件
    */

   ns2::func();
   return 0;
}