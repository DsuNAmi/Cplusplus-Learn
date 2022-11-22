//开启多个进程

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <errno.h>


#include "ngx_func.h"
#include "ngx_macro.h"
#include "ngx_c_conf.h"


//函数声明
static void NgxStartWorkProcess(int threadnums);
static int NgxSpawnProcess(int threadnums, const char * pProcName);
static void NgxWorkerProcessCycle(int inum, const char * pProcName);
static void NgxWorkerProcessInit(int inum);


//变量声明
static u_char masterProcess[] = "master process";

//创建worker子进程
void ngxMasterProcessCycle()
{
    sigset_t    set;        //信号集

    sigemptyset(&set);      //清空信号集

    //现在我们在该进程中屏蔽如下信号
    //建议fork()子进程时学习这种写法，防止信号的干扰；
    sigaddset(&set, SIGCHLD);     //子进程状态改变
    sigaddset(&set, SIGALRM);     //定时器超时
    sigaddset(&set, SIGIO);       //异步I/O
    sigaddset(&set, SIGINT);      //终端中断符
    sigaddset(&set, SIGHUP);      //连接断开
    sigaddset(&set, SIGUSR1);     //用户定义信号
    sigaddset(&set, SIGUSR2);     //用户定义信号
    sigaddset(&set, SIGWINCH);    //终端窗口大小改变
    sigaddset(&set, SIGTERM);     //终止
    sigaddset(&set, SIGQUIT);     //终端退出符

    //设置，此时无法接受的信号
    //阻塞期间，发过来的信号，多个会合并为一个，暂存着，等你放开信号屏蔽后才能收到信号

    //参数SIG——BLOCK表明设置 进程 新的信号屏蔽字为，当前信号屏蔽字和第二个参数指向的信号集的并集
    if(sigprocmask(SIG_BLOCK,&set, nullptr) == -1)
    {
        NgxLogErrorCore(NGX_LOG_ALERT,errno, "ngxMasterProcessCycle()'s sigprocmask() failed!");
    }

    //失败了不会阻止流程停止

    //首先设置主进程标题
    size_t size;
    int i;
    size = sizeof(masterProcess);       //这里字符串末尾的\0被计算进来

    size += gArgvneedmem;

    if(size < 1000)
    {
        char title[1000] = {0};
        strcpy(title,(const char*)masterProcess);
        strcat(title, " ");      //这里连接一个空格，之后把参数都拼过来
        for(int i = 0; i < g_os_argc; ++i)
        {
            strcat(title, g_os_argv[i]);
        }
        NgxSetproctitle(title);
    }

    //从配置文件中创建worker的数量
    CConfig * pConifg = CConfig::GetInstance();
    int workProcess = pConifg->GetIntDefault("WorkerProcesses",1);
    NgxStartWorkProcess(workProcess);   //创建进程


    //创建之后，父进程会回到这里，子进程不会
    sigemptyset(&set);      //不屏蔽信号
    
    for(;;)
    {
        // NgxLogErrorCore(0,0,"hahd this is father Processor pid = %P",ngxPid);
        sigsuspend(&set);
        sleep(1);
    }
    return;

}

//创建需要的子进程数量
static void NgxStartWorkProcess(int threadnums)
{
    int i;
    for(int i = 0; i < threadnums; ++i) //通过这个循环来创建进程
    {
        NgxSpawnProcess(i,"worker process");
    }
    return;
}

//产生一个子进程
static int NgxSpawnProcess(int threadnums, const char * pProcName)
{
    pid_t pid;

    pid = fork();
    switch(pid)
    {
        case -1:
            //失败
            NgxLogErrorCore(NGX_LOG_ALERT, errno, "ngxSpawnProcess() fork() create worker process num = %d, procName = \"%s\" failed!",threadnums, pProcName);
            return -1;
        case 0:
            //子进程的执行分支
            ngxParent = ngxPid;
            ngxPid = getpid();
            //所有的子进程进一个循环进行工作
            NgxWorkerProcessCycle(threadnums,pProcName);
            break;
        default:
            //父进程分支，直接break
            break;
    }


    //补充代码
    return pid;
}

static void NgxWorkerProcessCycle(int inum, const char * pProcName)
{
    //重新为子进程设置进程名

    ngxProcess = NGX_PROCESS_WORKER;
    NgxWorkerProcessInit(inum);
    NgxSetproctitle(pProcName);
    // NgxLogStderr(0,"what things");
    NgxLogErrorCore(NGX_LOG_NOTICE,0,"%s %P 启动并开始运行.....!",pProcName,ngxPid);


    for(;;)//放个四循环不让子进程出来
    {
        // NgxLogErrorCore(0,0,"good-this is a subPorcessor ID = %d, pid = %P !",inum, ngxPid);

        NgxProcessEventsAndTimes();
    }

    //如果跳出循环，停止线程
    gThreadPool.StopAll();
    gSocket.ShutDownSubProc();

    return;
}

static void NgxWorkerProcessInit(int inum)
{
    sigset_t set;
    sigemptyset(&set);

    //取消子进程的信号屏蔽
    if(sigprocmask(SIG_SETMASK,&set,nullptr) == -1)
    {
        NgxLogErrorCore(NGX_LOG_ALERT,errno,"ngx_worker_process_init()'s sigprocmask() failed!");
    }

    //线程池代码，率先创建，至少要比和socket相关优先
    CConfig * pConfig = CConfig::GetInstance();
    int tempThreadNums = pConfig->GetIntDefault("ProcMsgRecvWorkThreadCount",5);
    if(gThreadPool.Create(tempThreadNums) == false)
    {
        exit(-2);
    }
    sleep(1);

    if(gSocket.InitializeSubProc() == false)
    {
        exit(2);
    }

    gSocket.NgxEpollInit();

    return;
}