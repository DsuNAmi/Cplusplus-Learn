#include <iostream>

using namespace std;

union myun //以size最大的结构作为自己的大小
{
    struct 
    {
        int x;
        int y;
        int z;
    }u;

    int k;//所以k的地址其实就是x的地址
    
}a;

int main()
{
    a.u.x = 1;
    a.u.y = 5;
    a.u.z = 6;
    // a.k = 0; //覆盖屌第一个int空间值
    cout << a.u.x << " " << a.u.y << " " << a.u.z << " " << a.k << endl;
    cout << &a.u.x << " " << &a.k << endl;
    return 0; 
}
