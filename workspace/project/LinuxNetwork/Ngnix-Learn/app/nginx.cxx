#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <arpa/inet.h>

#include "ngx_func.h"
#include "ngx_c_conf.h"
#include "ngx_macro.h"
#include "ngx_c_socket.h"
#include "ngx_c_memory.h"
#include "ngx_c_threadpool.h"
#include "ngx_c_crc32.h"
#include "ngx_c_slogic.h"


static void FreeSource();


size_t gArgvneedmem = 0;
size_t gEnvneedmem = 0;
char **g_os_argv;
char *gp_envmem = nullptr;
int g_environlen = 0;
int g_os_argc = 0;
int gDaemonized = 0;

// CSocket gSocket;
CLogicSocket gSocket;

pid_t ngxPid;
pid_t ngxParent;
int ngxProcess;
int gStopEvent;

sig_atomic_t ngxReap;


int main(int argc, char * const * argv)
{

    int exitcode = 0;
    int i;
    ngxPid = getpid();
    ngxParent = getppid();

    gArgvneedmem = 0;
    for(i = 0; i < argc; ++i)
    {
        gArgvneedmem += strlen(argv[i]) + 1;
    }
    for(i = 0; environ[i]; ++i)
    {
        gEnvneedmem += strlen(environ[i]) + 1;
    }




    g_os_argc = argc;
    g_os_argv = (char **)argv;


    ngxLog.fd = -1;     //这里表示文件还没打开，一个初始化
    ngxProcess = NGX_PROCESS_MASTER;
    ngxReap = 0;        //标记子进程没有变化



    // NgxLogStderr(0,"invalid option: %s, %d", "testinfo", 123);
    // NgxLogStderr(0,"invalid option: %10d",21);
    // NgxLogStderr(0,"invalid option: %.6f",21.378);
    // NgxLogStderr(0,"invalid option: %.2f",12.999);
    // NgxLogStderr(0,"invalid option: %xd",1678);
    // NgxLogStderr(0,"invalid option: %Xd",1678);
    // NgxLogStderr(15,"invalid option: %s, %d", "testInfo",326);

    // if(true)
    // {
    //     printf("test memory \n");
    //     return 0;
    // }

    // printf("environ[0] = %s\n",environ[0]);
    // printf("environ[1] = %s\n",environ[1]);
    // printf("environ[2] = %s\n",environ[2]);
    // printf("environ[3] = %s\n",environ[3]);
    // printf("environ[4] = %s\n",environ[4]);
    

    // printf("argc=%d, argv[0]=%s\n",argc,argv[0]);

    // strcpy(argv[0],"ce");


    // for(int i = 0; environ[i]; ++i)
    // {
    //     printf("environ[%d]地址 = %x\n",i, (unsigned int)((unsigned long)environ[i]));
    //     printf("environ[%d]内容 = %s\n",i,environ[i]);
    // }

    // printf("----------------------------");


    // for(int i = 0; environ[i]; ++i)
    // {
    //     printf("environ[%d]地址 = %x\n",i, (unsigned int)((unsigned long)environ[i]));
    //     printf("environ[%d]内容 = %s\n",i,environ[i]);
    // }


    CConfig * pConfig = CConfig::GetInstance();
    if(pConfig->Load("nginx.conf") == false)
    {
        NgxLogInit();       //因为有个缺省的日志目录，所以就是可以作为写入日志的，不然就没有日志目录了
        NgxLogStderr(0,"Config FIle [%s], load falied!","nginx.conf");
        exitcode = 2;
        goto lblexit;
    }

    CMemory::GetInstance();
    //crc初始化
    CCRC32::GetInstance();

    NgxLogInit();
    if(ngxInitSignals() != 0)
    {
        exitcode = 1;
        goto lblexit;
    }


    if(false == gSocket.Initialize())
    {
        exitcode = 1;
        goto lblexit;
    }





    // printf("test for starting\n");
    // mysignal();

    // for(;;)
    // {
    //     sleep(1);
    //     printf("rest one second\n");
    // }




    NgxInitSetproctitle();
    // NgxSetproctitle("nginx: master process");


    // NgxLogErrorCore(5,8,"some error in here, show the value = %s", "YYYY");

    // for(;;)
    // {
    //     sleep(1);
    //     printf("rest one second\n");
    // }


    //创建守护进程
    if(pConfig->GetIntDefault("Daemon",0) == 1)
    {
        //进来就是按照守护进程方式进行
        int cDaemonResult = ngxDaemon();
        if(-1 == cDaemonResult)
        {
            exitcode = 1;
            goto lblexit;
        }
        if(1 == cDaemonResult)
        {
            //父进程，释放资源
            FreeSource();
            exitcode = 0;
            return exitcode;
        }

        //走到这里说明是子进程，所以标记守护进程
        gDaemonized = 1;
        //这里主要是把原来直接运行的主进程down了，作了一个新的主进程，作为创建守护进程的主进程
        //所以这样的话，就连主进程的终端也是问号
    }

    ngxMasterProcessCycle();

lblexit:
    NgxLogStderr(0,"code exit, goodbye!");      //这是因为怕直接输出到黑洞中了
    FreeSource();
    return exitcode;
}

void FreeSource()
{
    if(gp_envmem)
    {
        delete [] gp_envmem;
        gp_envmem = nullptr;
    }

    if(ngxLog.fd != STDERR_FILENO && ngxLog.fd != -1)
    {
        close(ngxLog.fd);
        ngxLog.fd = -1;
    }
}