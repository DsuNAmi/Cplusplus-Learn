/* 
仿函数
*/

#include <iostream>

using namespace std;

template <class T>
struct identity{
    const T& operator()(const T& x) const {return x;}
};

int main()
{
    int b = 1;
    
    identity<int> idt;

    int a = idt(b);

    cout << a << endl;
    
    return 0;
}