/**
 * @file func_pointer.cpp
 * @author your name (you@domain.com)
 * @brief 函数指针
 * @version 0.1
 * @date 2022-07-07
 * 
 * @copyright Copyright (c) 2022
 * 
 */


#include <iostream>

using namespace std;

/**
 * @brief 定义了一个变量pFun,这个变量是一个指针，指向返回值为空和参数为int的函数指针！
 * 
 */
void (*pFun)(int);


/**
 * @brief 代表了一种新类型，不是变量，所以与上述的pFun不一样
 * 别名为func，然后是一种函数指针，返回值为void,参数为void
 */
typedef void (*func)(void);

void myfunc(void)
{
    cout << "asda" << endl;
}

void glFun(int a) {cout << a << endl;}




int main()
{
    func pfun = myfunc;
    pfun();
    pFun = glFun;
    (*pFun)(2);
    return 0;
}