#ifndef __NGX_GBLDEF_H__
#define __NGX_GBLDEF_H__


#include <signal.h>

// #include "ngx_c_socket.h"
#include "ngx_c_threadpool.h"
#include "ngx_c_slogic.h"

typedef struct
{
    char itemName[50];
    char itemContent[500];

}CConfItem, *LPCConfItem;

typedef struct
{
    int logLevel;       //日志级别
    int fd;             //日志文件描述符
} ngxLogT;


extern int g_os_argc;
extern char **g_os_argv;
extern char *gp_envmem;
extern int g_environlen;
extern size_t gArgvneedmem;
extern size_t gEnvneedmem;

extern pid_t ngxPid;
extern pid_t ngxParent;

extern ngxLogT ngxLog;

extern int gDaemonized;
extern int ngxProcess;
extern sig_atomic_t ngxReap;

// extern CSocket gSocket;
extern CLogicSocket gSocket;
extern CThreadPool gThreadPool;

extern int          gStopEvent;

#endif