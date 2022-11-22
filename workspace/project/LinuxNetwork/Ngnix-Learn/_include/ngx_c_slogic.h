#ifndef __NGX_C_SLOGIC_H__
#define __NGX_C_SLOGIC_H__

#include <sys/socket.h>
#include "ngx_c_socket.h"


//处理逻辑和通讯的子类
class CLogicSocket : public CSocket
{
    public:
        CLogicSocket();
        virtual ~CLogicSocket();
        virtual bool Initialize();

        //通用收发数据相关函数
        void SendNoBodyPkgToClient(LPSTRUC_MSG_HEADER pMsgHeader, unsigned short iMsgCode);

        //各种业务逻辑相关函数
        bool _HandleResister(lpngxConnectionT pConn, LPSTRUC_MSG_HEADER pMsgHeader, char * pPkgBody, unsigned short iBodyLength);
        bool _HandleLogin(lpngxConnectionT pConn, LPSTRUC_MSG_HEADER pMsgHeader, char * pPkgBody, unsigned short iBodyLength);
        bool _HandlePing(lpngxConnectionT pConn, LPSTRUC_MSG_HEADER pMSgHeader, char * pPkgBody, unsigned short iBodyLength);

        //心跳包时间检测
        virtual void ProcPingTimeOutChecking(LPSTRUC_MSG_HEADER tempMsg, time_t curTime);

        virtual void ThreadRecvProcFunc(char * pMsgBug);
};

#endif