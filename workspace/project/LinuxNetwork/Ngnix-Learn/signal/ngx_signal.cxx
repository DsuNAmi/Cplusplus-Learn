#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>

#include "ngx_macro.h"
#include "ngx_func.h"
#include "ngx_global.h"

//写一个和信号有关的结构
typedef struct
{
    int         signo;      //信号对应的数字缩写
    const char  *sigName;   //信号对应的名字

    //信号处理函数，这个函数由我们自己提供，但是它的参数和返回值是固定的
    void (*handler)(int signo, siginfo_t * siginfo, void * ucontext);  //函数指针
} ngxSignalT;


//声明一个信号处理函数，static 表示在当前文件中可见
static void ngxSignalHandler(int signo, siginfo_t * siginfo, void * ucontext);
static void ngxProcessGetStatus(void);  //防止产生僵尸进程


ngxSignalT signals[] = {
    //signo         signame             handler
    {SIGHUP,        "SIGHUP",           ngxSignalHandler},  //终端断开信号，对于守护进程常用于reload重载配置文件
    {SIGINT,        "SIGINT",           ngxSignalHandler},  //标识2
    {SIGTERM,       "SIGTERM",          ngxSignalHandler},  //标识15
    {SIGCHLD,       "SIGCHLD",          ngxSignalHandler},  //子进程推出时，父进程会收到这个信号
    {SIGQUIT,       "SIGQUIT",          ngxSignalHandler},  //标识3
    {SIGIO,         "SIGIO",            ngxSignalHandler},  //指一个异步I/O事件
    {SIGSYS,        "SIGSYS, SIG_IGN",  nullptr},   //我们需要忽略这个信号，SIGSYS表示收到了一个无效系统调用，如果我们不忽略，就会被操作系统杀死
                                                    //所以我们把句柄设置为nullptr，要求忽略这个信号
    //...以后添加
    {0 ,            nullptr,            nullptr}    //信号对应的数字至少是1,所以0可以是一个特殊标记
};


//成功返回0,失败返回-1
int ngxInitSignals()
{
    ngxSignalT      *sig;
    struct sigaction sa;        //系统根信号有关的函数

    for(sig = signals; sig->signo !=0; ++sig)   //0刚好可以终止这个信号操作
    {
        memset(&sa, 0, sizeof(struct sigaction));

        if(sig->handler) //如果句柄不为空   
        {
            sa.sa_sigaction = sig->handler; //指定信号处理函数
            sa.sa_flags = SA_SIGINFO;       //表示信号附带的参数可以被传递到信号处理函数中
                                            //为了让自定义函数生效，必须用这个
        }
        else
        {
            sa.sa_handler = SIG_IGN;        //忽略信号的处理
        }

        // 比如我们处理某个信号，比如SIGUSR1信号时不希望SIGUSR2信号，那我们就可以用sigaddset(&sa, sa_mask, SIGUSR2)，
        // sa_mask就是要阻塞的信号
        sigemptyset(&sa.sa_mask);       //把信号集清零，表示不阻塞任何信号

        //信号处理函数
        //参数1,要操作的信号
        //参数2,针对信号的信号处理和函数，以及要屏蔽的信号
        //参数3,返回以往对应信号的处理方式，我们不需要
        if(sigaction(sig->signo, &sa, nullptr) == -1)
        {
            NgxLogErrorCore(NGX_LOG_EMERG,errno,"sigaction(%s) failed",sig->sigName);
            return -1;      //有错误直接返回
        }
        else
        {
            // NgxLogStderr(0,"sigaction(%s) succeed!", sig->sigName);
        }
    }

    
    return 0;
}


static void ngxSignalHandler(int signo, siginfo_t * siginfo, void * ucontext)
{
    // printf("receive a signal!\n");
    ngxSignalT  *sig;
    char        *action;        //写入日志

    for(sig = signals; sig->signo; ++sig)
    {
        if(sig->signo == signo)
        {
            break; //处理该信号，主要是把sig的指针指对
        }
    }

    action = (char*)"";

    if(ngxProcess == NGX_PROCESS_MASTER)
    {
        //主进程
        switch(signo)
        {
            case SIGCHLD:
                ngxReap =   1;  //标记子进程发生变化
                break;;
            
            default:
                break;
        }
    }
    else if(ngxProcess == NGX_PROCESS_WORKER)
    {
        //子进程
    }
    else
    {
        //其他情况不管
    }

    if(siginfo && siginfo->si_pid) //si_pid = sending process
    {
        NgxLogErrorCore(NGX_LOG_NOTICE,0,"signal %d (%s) received from %P%s", signo, sig->sigName, siginfo->si_pid,action);
    }
    else
    {
        NgxLogErrorCore(NGX_LOG_NOTICE,0,"signal %d (%s) received %s",signo,sig->sigName,action);
    }


    if(signo == SIGCHLD)
    {
        ngxProcessGetStatus();
    }

    // sleep(10);
    return;
}

static void ngxProcessGetStatus(void)
{
    pid_t           pid;
    int             status;
    int             err;
    int             one = 0; 


    for(;;)
    {
        pid = waitpid(-1,&status,WNOHANG);

        if(0 == pid)
        {
            return;
        }
        if(pid == -1)   //表示waipid有错误
        {
            err = errno;
            if(err == EINTR)    //调用被某个信号中断
            {
                continue;
            }
    
            if(err == ECHILD && one)    //没有子进程
            {
                return;
            }

            if(err = ECHILD)            //没有子进程
            {
                NgxLogErrorCore(NGX_LOG_INFO,err, "waitpid() failed");
                return;
            }

            NgxLogErrorCore(NGX_LOG_ALERT,err,"waitpid() failed!");
            return;
        }

        //走到这里表示，成功返回进程ID
        one = 1;

        if(WTERMSIG(status))    //获取使子进程终止的信号编号
        {
            NgxLogErrorCore(NGX_LOG_ALERT,0,"pid= %P exited on signal %d!",pid,WTERMSIG(status));
        }
        else
        {
            NgxLogErrorCore(NGX_LOG_NOTICE,0,"pid= %P exited with code %d!",pid,WTERMSIG(status));

        }
    }

    return;
}