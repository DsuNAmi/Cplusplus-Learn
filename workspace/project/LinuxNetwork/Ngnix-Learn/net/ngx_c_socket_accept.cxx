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
#include <sys/ioctl.h>
#include <arpa/inet.h>


#include "ngx_c_conf.h"
#include "ngx_macro.h"
#include "ngx_global.h"
#include "ngx_func.h"
#include "ngx_c_socket.h"


void CSocket::NgxEventAccept(lpngxConnectionT oldc)
{
    //
    struct sockaddr mySockAddr; //远端服务器的socket地址
    socklen_t       sockLen;
    int             err;
    int             level;
    int             s;
    static int      useAccept4 = 1;
    lpngxConnectionT newc;


    sockLen = sizeof(mySockAddr);

    do
    {
        if(useAccept4)
        {
            s = accept4(oldc->fd, &mySockAddr, &sockLen, SOCK_NONBLOCK); 
            //最后一个参数表示返回一个非阻塞的socket，节省一次iotcl调用
        }
        else
        {
            accept(oldc->fd, &mySockAddr, &sockLen);
            //这里就是TCP三次握手会获得其IP地址和端口号
        }

        if(-1 == s) //这里是返回失败的原因
        {
            err = errno;
            if(err == EAGAIN)
            {
                //accept没有准备好，
                return;
            }
            level = NGX_LOG_ALERT;
            
            if(err == ECONNABORTED) //发生在意外关闭套接字后
            {
                //该错误被描述为软件引起的连接中止
                level = NGX_LOG_ERR;
            }
            else if(err == EMFILE || err == ENFILE)
            {
                //进程的号码用尽
                level = NGX_LOG_CRIT;
            }

            NgxLogErrorCore(level,errno,"CSocket::NgxEventAccept accept4 failed");

            if(useAccept4 && err == ENOSYS)
            {
                //accept4失效
                useAccept4 = 0;
                continue;
            }

            if(err == ECONNABORTED)
            {

            }

            if(err == EMFILE || err == ENFILE)
            {

            }

            return;
        }


        if(mOnlineUserCount >= mWorkerConnections)
        {
            //超过了最大可连接数量
            NgxLogStderr(0,"upper the capacity, (%d), close the connection(%d)",mWorkerConnections,s);
            close(s);
            return;
        }

        //表示accpet成功
        newc = NgxGetConnection(s);
        if(newc == nullptr)
        {
            //连接池不够用
            if(close(s) == -1)
            {
                NgxLogErrorCore(NGX_LOG_ALERT,errno,"CSocket::NgxEventAccept() 's close(%d)",s);
                
            }
            return;
        }

        //成功拿到了连接池中的一个连接
        memcpy(&newc->sSockAddr,&mySockAddr,sockLen);


        if(!useAccept4)
        {
            if(Setnonblocking(s) == false)
            {
                //设置非阻塞失败
                //关闭连接，释放资源
                // NgxCloseAcceptedConnection(newc);
                NgxCloseConnection(newc);
                return;
            }
        }

        //这个newc是客户端的那个套接字，oldc是服务器的套接字对应的
        newc->listening = oldc->listening;
        // newc->wReady = 1;
        newc->rHandler = &CSocket::NgxReadRequestHandler;
        newc->wHandler = &CSocket::NgxWriteRequestHandler;

        // if(NgxEpollAddEvent(s,
        //                     1,0,
        //                     0,
        //                     EPOLL_CTL_ADD,
        //                     newc) == -1)   //读，写
        // {
        //     // NgxCloseAcceptedConnection(newc);
        //     NgxCloseConnection(newc);
        //     return;
        // }

        if(NgxEpollOperEvent(s,
                             EPOLL_CTL_ADD,
                             EPOLLIN | EPOLLRDHUP,
                             0,
                             newc) == -1)
        {
            NgxCloseConnection(newc);
            return;
        }

        if(1 == mifKickTimeCount)
        {
            AddToTimeQueue(newc);
        }

        ++mOnlineUserCount;     //连入数量加一

        break;             
    } while (1);
    
    return;
}

// void CSocket::NgxCloseAcceptedConnection(lpngxConnectionT c)
// {
//     int fd = c->fd;
//     NgxFreeConnection(c);
//     c->fd = -1;
//     if(close(fd) == -1)
//     {
//         NgxLogErrorCore(NGX_LOG_ALERT,errno,"CSocket::NgxCloseAcceptedConnetion close(%d) failed",fd);

//     }
//     return;
// }