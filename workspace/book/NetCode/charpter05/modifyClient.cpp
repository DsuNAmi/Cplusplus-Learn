#include <iostream>
#include <winsock2.h>

#include "error4.h"

using namespace std;


const int BUFFSIZE = 1024;

int main(int argc, char * argv[])
{
    WSADATA wsadata;
    SOCKET clientSock;
    SOCKADDR_IN servAddr;


    if(argc != 3) ParamsFormat(argv[0],CLIENT);

    if(WSAStartup(MAKEWORD(2,2),&wsadata)) ErrorHandling(StartUpError);

    clientSock = socket(PF_INET,SOCK_STREAM,0);
    if(clientSock == INVALID_SOCKET) ErrorHandling(SocketError);

    memset(&servAddr,0,sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = inet_addr(argv[1]);
    servAddr.sin_port = htons(atoi(argv[2]));

    if(connect(clientSock,(SOCKADDR*)&servAddr,sizeof(servAddr)) == SOCKET_ERROR) ErrorHandling(ConnectError);
    else printf("Connecting....");


    while(1)
    {
        char message[BUFFSIZE];
        fputs("Input Message(Q/q to quit): ",stdout);
        fgets(message,BUFFSIZE,stdin);

        if(!strcmp(message,"q\n") || !strcmp(message,"Q\n")) break;

        int recvAllLen = 0;
        int sendLen = send(clientSock,message,strlen(message),0);
        while(recvAllLen < sendLen)
        {
            int recvLen = recv(clientSock, message, BUFFSIZE - 1, 0);
            if(recvLen == SOCKET_ERROR) ErrorHandling(RecvError);
            recvAllLen += recvLen;
        }
        message[recvAllLen] = 0;
        printf("Message from server: %s",message);
    }
    closesocket(clientSock);
    WSACleanup();

    return 0;
}