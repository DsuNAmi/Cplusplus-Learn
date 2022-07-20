#include <iostream>
#include <winsock2.h>

#include "error5.h"

int main()
{
    WSADATA wsadata;
    SOCKET servSocket;
    SOCKADDR_IN servAddr, clntAddr;

    if(WSAStartup(MAKEWORD(2,2),&wsadata)) ErrorHandling(StartUpError);

    servSocket = socket(PF_INET, SOCK_DGRAM, 0);
    if(servSocket == INVALID_SOCKET) ErrorHandling(SocketError);

    memset(&servAddr,0,sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(atoi(PORT));

    if(bind(servSocket,(SOCKADDR*)&servAddr,sizeof(servAddr)) == SOCKET_ERROR) ErrorHandling(BindError);

    //recv and send;
    char message[BUFFSIZE];
    int times = 3;
    while(times > 0)
    {
        int sizeClnt = sizeof(clntAddr);
        int rl = recvfrom(servSocket,message,BUFFSIZE,0,(SOCKADDR*)&clntAddr,&sizeClnt);
        if(rl == SOCKET_ERROR) ErrorHandling(RecvError);
        int sl = sendto(servSocket,message,strlen(message),0,(SOCKADDR*)&clntAddr,sizeClnt);
        --times;
    }

    closesocket(servSocket);
    WSACleanup();

    return 0;
}