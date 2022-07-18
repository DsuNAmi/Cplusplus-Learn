#include <iostream>
#include <winsock2.h>

#include "error3.h"


using namespace std;

const int BUFFSIZE = 1024;


int main(int argc, char * argv[])
{
    WSADATA wsadata;
    SOCKET servSocket, clntScocket;
    sockaddr_in servAddr, ClntAddr;

    if(argc != 2) ParamsFormat(argv[0],SERVER);

    if(WSAStartup(MAKEWORD(2,2),&wsadata)) ErrorHandling(StartUpError);


    servSocket = socket(PF_INET,SOCK_STREAM,0);
    if(servSocket == INVALID_SOCKET) ErrorHandling(SocketError);

    memset(&servAddr,0,sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(atoi(argv[1]));


    if(bind(servSocket,(SOCKADDR *)&servAddr,sizeof(servAddr)) == SOCKET_ERROR) ErrorHandling(BindError);

    if(listen(servSocket,5) == SOCKET_ERROR) ErrorHandling(ListenError);

    int sizeClntAddr = sizeof(ClntAddr);

    for(int i = 0; i < 5; ++i)
    {
        clntScocket = accept(servSocket,(SOCKADDR*)&ClntAddr,&sizeClntAddr);
        if(clntScocket == INVALID_SOCKET) ErrorHandling(AcceptError);
        else printf("Connect client %d \n", i + 1);

        int acceptLen = 0;
        char message[BUFFSIZE];
        while((acceptLen = recv(clntScocket,message,BUFFSIZE,0)) != 0)
            send(clntScocket,message,acceptLen,0);

        closesocket(clntScocket);

    }

    closesocket(servSocket);
    WSACleanup();
    return 0;
}