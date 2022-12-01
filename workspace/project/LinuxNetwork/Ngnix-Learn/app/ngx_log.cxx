#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>

#include "ngx_global.h"
#include "ngx_macro.h"
#include "ngx_func.h"
#include "ngx_c_conf.h"


//全局量
static u_char errLevels[][20] = 
{
    {"stderr"}, //0 控制台错误
    {"emerg"},  //1 紧急
    {"alert"},  //2 警戒
    {"crit"},   //3 严重
    {"error"},  //4 错误
    {"warn"},   //5 警告
    {"notice"}, //6 注意
    {"info"},   //7 信息
    {"debug"},  //8 调试
};


ngxLogT ngxLog;
/// @brief 打印结果到日志中
/// @param err , 日志等级
/// @param fmt , 输出的字符串格式
/// @param printResult(...), 这里是要打印的变量
void NgxLogStderr(int err, const char * fmt, ...)
{
    va_list args;           //创建一个va_list变量
    u_char errstr[NGX_MAX_ERROR_STR + 1];
    u_char *p;
    u_char *last;


    memset(errstr, 0, sizeof(errstr));

    last = errstr + NGX_MAX_ERROR_STR;      //这里主要是一个标记，防止输出内容超过这个长度
    
    p = ngx_memcpy(errstr, "nginx: ", 7);   //这里p的长度注意不是首地址，而是最后

    va_start(args, fmt); //这句话的作用就是让args指向fmt参数的后面，因为是+一个fmt的大小，这样就直接直到所有可变参数的首地址
    
    p = NgxVSLPrintf(p,last,fmt,args);

    va_end(args);       //释放

    if(err)
    {
        //日志等级，如果是有错误，按照错误的情况处理
        //这里是要显示错误信息
        p = NgxLogErrno(p, last, err);
    }


    if(p >= (last - 1))
    {
        p = (last - 1) - 1;
    }
    *p++ = '\n';    

    //往标准错误输出【一般是屏幕】
    write(STDERR_FILENO, errstr, p - errstr);


    //输出写到日志文件中
    if(ngxLog.fd > STDERR_FILENO)
    {
        NgxLogErrorCore(NGX_LOG_STDERR,err,(const char *)errstr);
    }

    return;
}

u_char * NgxLogErrno(u_char * buf, u_char * last, int err)
{
    char *pErrorInfo = strerror(err);
    size_t len = strlen(pErrorInfo);

    char leftStr[10] = {0};
    sprintf(leftStr," (%d: ", err);
    size_t leftLen = strlen(leftStr);

    char rightStr[] = ") ";
    size_t rightLen = strlen(rightStr);

    size_t extraLen = leftLen + rightLen;
    if((buf + len + extraLen ) < last)
    {
        buf = ngx_memcpy(buf,leftStr,leftLen);
        buf = ngx_memcpy(buf, pErrorInfo, len);
        buf = ngx_memcpy(buf, rightStr, rightLen);
    }


    return buf;
}

//这个往日志里写对应的内容，因为代码中在调用自己写函数时，已经是有换行符了，所以就也不需要加
void NgxLogErrorCore(int level, int err, const char * fmt, ...)
{
    u_char * last;
    u_char errstr[NGX_MAX_ERROR_STR + 1];

    memset(errstr, 0, sizeof(errstr));

    last = errstr + NGX_MAX_ERROR_STR;

    struct timeval  tv;
    struct tm       tm;

    time_t          sec;
    u_char          *p;
    va_list         args;

    memset(&tv, 0, sizeof(struct timeval));
    memset(&tm, 0, sizeof(struct tm));


    gettimeofday(&tv,nullptr);      //获取当前之前，自1970-01-01的时间，第二个是时区

    sec = tv.tv_sec;
    localtime_r(&sec, &tm);         //转换为本地时间放到第二个参数中
    //这里是修改一下才能是对的
    tm.tm_mon++;
    tm.tm_year += 1900;

    u_char structTime[40] = {0};        //构造一个格式的字符串，比如：2021/11/2/ 13:43:11
    NgxSLPrintf(structTime,
                (u_char*) - 1,          //这是一个很大的值
                "%4d/%02d/%02d %02d:%02d:%02d",
                tm.tm_year,tm.tm_mon,
                tm.tm_mday,tm.tm_hour,
                tm.tm_min,tm.tm_sec);   

    p = ngx_memcpy(errstr, structTime, strlen((const char *)structTime));
    p = NgxSLPrintf(p, last, " [%s] ", errLevels[level]);
    p = NgxSLPrintf(p, last, "%p: ", ngxPid);


    va_start(args, fmt);
    p = NgxVSLPrintf(p, last, fmt, args);
    va_end(args);

    if(err)
    {
        p = NgxLogErrno(p, last, err);
    }

    if(p >= (last - 1))
    {
        p = (last - 1) - 1;
    }
    *p++ = '\n';

    ssize_t n;

    while(1)
    {
        if(level > ngxLog.logLevel)
        {
            break;      //这种等级的日志就不打印了
        }

        n = write(ngxLog.fd, errstr, p - errstr);
        if(-1 == n)
        {
            if(errno == ENOSPC)
            {
                //磁盘没空间了
            }
            else
            {
                if(ngxLog.fd != STDERR_FILENO)
                {
                    //写到屏幕上去
                    n = write(STDERR_FILENO, errstr, p - errstr);
                }
            }
        }
        break;
    }
    
    return;
}

void NgxLogInit()
{
    u_char * pLogName = nullptr;
    size_t nlen;

    CConfig * pConfig = CConfig::GetInstance();
    pLogName = (u_char *)pConfig->GetString("Log");     //直接读路径名

    if(pLogName == nullptr)
    {
        //读文件失败的话就试一试默认的文件名
        pLogName = (u_char*) NGX_ERROR_LOG_PATH;
    }

    ngxLog.logLevel = pConfig->GetIntDefault("Loglevel",NGX_LOG_NOTICE);        //有个缺省的值
    //只写、追加、创建、并设置文件权限
    ngxLog.fd = open((const char *)pLogName, O_WRONLY | O_APPEND | O_CREAT,0644);

    if(ngxLog.fd == -1)
    {
        NgxLogStderr(errno, "[alert] could not open error log file : open() \"%s\" failed", pLogName);
        ngxLog.fd = STDERR_FILENO;
    }

    return;
}

