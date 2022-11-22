#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "ngx_global.h"

//设置可执行程序标题相关函数：分配内存，并且把环境变量拷贝到新内存中来
void NgxInitSetproctitle()
{
    int i;
    //统计环境变量的内存
    for(i = 0; environ[i]; ++i)
    {
        g_environlen += strlen(environ[i]) + 1;//这里是因为\0占一个内存
    }

    gp_envmem = new char[g_environlen];
    memset(gp_envmem,0,g_environlen);

    char * pTemp = gp_envmem;

    //把原来的内存内容搬到新地方来
    for(i = 0; environ[i]; ++i)
    {
        size_t size = strlen(environ[i]) + 1;
        strcpy(pTemp, environ[i]);
        environ[i] = pTemp;         //环境变量指向这块新内存
        pTemp += size;
    }

    return;
}


//设置可执行标题
void NgxSetproctitle(const char * title)
{
    size_t ititlelen = strlen(title);

    size_t e_environ = 0;           //e表示局部变量

    for(int i = 0; g_os_argv[i]; ++i)
    {
        e_environ += strlen(g_os_argv[i]) + 1;
    }

    size_t esy = e_environ + g_environlen;

    if(esy <= ititlelen)
    {
        //这里是标题太长的情况
        return;
    }

    g_os_argv[1] = nullptr;

    char * pTemp = g_os_argv[0];
    strcpy(pTemp, title);
    pTemp += ititlelen;

    //把后面的内容全部清零
    size_t remain = esy - ititlelen;
    memset(pTemp, 0, remain);
    return;
}