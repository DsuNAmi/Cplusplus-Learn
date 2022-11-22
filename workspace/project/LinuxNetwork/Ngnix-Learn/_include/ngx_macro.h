#ifndef __NGX_MACRO_H__
#define __NGX_MACRO_H__


#define NGX_MAX_ERROR_STR 2048           //错误信息里量的最大长度

//类似memcpy,但常规memcpy返回的是指向目标dst的指针
//而我们可以自定义返回拷贝后数据的终点位置
#define ngx_memcpy(dst, src, n) (((u_char *) memcpy(dst,src, n)) + (n))
#define ngx_min(val1,val2) ((val1 > val2) ? (val2) : (val1))

#define NGX_MAX_UINT32_VALUE (uint32_t) 0xffffffff
#define NGX_INT64_LEN (sizeof("-9223372036854775808") - 1)

//日志
//我们把日志分成8个等级，【级别从高到低，数字最小的级别最高，数字大的级别最低】，方便管理、显示、过滤
#define NGX_LOG_STDERR              0       //控制台错误
#define NGX_LOG_EMERG               1       //紧急
#define NGX_LOG_ALERT               2       //警戒
#define NGX_LOG_CRIT                3       //严重
#define NGX_LOG_ERR                 4       //错误
#define NGX_LOG_WARN                5       //警告
#define NGX_LOG_NOTICE              6       //注意
#define NGX_LOG_INFO                7       //信息
#define NGX_LOG_DEBUG               8       //调试


#define NGX_ERROR_LOG_PATH          "logs/error.log"       //路径

//进程
#define NGX_PROCESS_MASTER      0 //MASTER进程，管理进程
#define NGX_PROCESS_WORKER      1 //WORKER进程，工作进程

#endif