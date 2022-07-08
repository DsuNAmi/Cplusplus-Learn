/**
 * @file blankclass.cppp
 * @brief 空类的大小为1字节
 * @version v1
 * @date 2022-07-07
 * 
 * @copyright Copyright (c) 2022
 * 
 */


#include <iostream>

using namespace std;

class A
{};


int main()
{
    cout << sizeof(A) << endl;
    return 0;    
}