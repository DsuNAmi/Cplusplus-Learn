#ifndef __NGX_FUNC_H__
#define __NGX_FUNC_H__

//字符串处理函数
void Rtrim(char * string);
void Ltrim(char * string);

//标题
void NgxSetproctitle(const char * title);
void NgxInitSetproctitle();

//日志
void NgxLogInit();
void NgxLogStderr(int err, const char * fmt, ...);
void NgxLogErrorCore(int level, int err, const char * fmt, ...);

u_char * NgxLogErrno(u_char * buf, u_char * last, int err);
u_char * NgxSLPrintf(u_char * buf, u_char * last, const char * fmt, ...);
u_char * NgxSNPrintf(u_char * buf, size_t max, const char * fmt, ...);
u_char * NgxVSLPrintf(u_char * buf, u_char * last, const char * fmt, va_list args);

//信号流程相关
int ngxInitSignals();
void ngxMasterProcessCycle();
int ngxDaemon();
void NgxProcessEventsAndTimes();


#endif