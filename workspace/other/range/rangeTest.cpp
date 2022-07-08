/**
 * @file rangeTest.cpp
 * @author your name (you@domain.com)
 * @brief ::作用域解析符
 * @version 0.1
 * @date 2022-07-08
 * 
 * @copyright Copyright (c) 2022
 * 
 */



#include <iostream>
using namespace std;

int count = 0;

class A
{
    public:
        static int count; //类A的count
};

int A::count;

int main()
{
    ::count = 1;//设置全局的count
    A::count = 5;
    int count = 3;
    count = 4;
    cout << A::count << endl;
    cout << count << endl;
    cout << ::count << endl;
    return 0;
}