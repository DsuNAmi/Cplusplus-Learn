//守护进程

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>


#include "ngx_func.h"
#include "ngx_macro.h"
#include "ngx_c_conf.h"

//初始化守护进程
//失败返回-1,子进程返回0,父进程返回1

int ngxDaemon()
{
    switch(fork())
    {
        case -1:
            NgxLogErrorCore(NGX_LOG_EMERG,errno,"ngxDaemon()' fork() failed!");
            return -1;
        case 0:
            break;
        default:
            //父进程释放资源再杀死
            return 1;
    }

    //这里只有子进程才能走到这个流程
    ngxParent = ngxPid;
    ngxPid = getpid();

    if(-1 == setsid())
    {
        NgxLogErrorCore(NGX_LOG_EMERG,errno,"ngxDaemon's setsid() failed!");
        return -1;
    }

    //不要限制文件权限
    umask(0);


    //打开黑洞设备，读写方式打开
    int fd = open("/dev/null",O_RDWR);
    
    if(-1 == fd)
    {
        NgxLogErrorCore(NGX_LOG_EMERG,errno,"ngxDaemon()'s open() failed");
        return -1;
    }
    if(dup2(fd,STDIN_FILENO) == -1) //关闭这个，让黑洞设备成为标准输入
    {
        NgxLogErrorCore(NGX_LOG_EMERG,errno,"ngxDaemon()'s dup2(STDIN) failed");
        return -1;
    }
    if(dup2(fd,STDOUT_FILENO) == -1)
    {
        NgxLogErrorCore(NGX_LOG_EMERG,errno,"ngxDaemon()'s dup2(STDOUT) failed");
        return -1;
    }

    if(fd > STDERR_FILENO)
    {
        if(close(fd) == -1)
        {
            NgxLogErrorCore(NGX_LOG_EMERG,errno,"ngxDaemon()'s close() failed");
            return -1;
        }
    }

    return 0;
}