#include <iostream>
#include <winsock2.h>

#include "error2.h"

using namespace std;




int main(int argc, char * argv[])
{
    WSADATA wsadata;
    SOCKET servSocket, clntSocket;
    sockaddr_in servAddr, clntAddr;

    if(argc != 2)
    {
        ParamsFormat(argv[0],SERVER);
    }

    if(WSAStartup(MAKEWORD(2,2),&wsadata)) ErrorHandling(StartUpError);

    servSocket = socket(PF_INET,SOCK_STREAM,0);
    if(servSocket == INVALID_SOCKET) ErrorHandling(SocketError);
    

    memset(&servAddr,0,sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(atoi(argv[1]));


    if(bind(servSocket,(SOCKADDR*)&servAddr,sizeof(servAddr)) == SOCKET_ERROR) ErrorHandling(SocketError);

    if(listen(servSocket,5) == SOCKET_ERROR) ErrorHandling(ListenError);


    int sizeClnt = sizeof(clntAddr);
    clntSocket = accept(servSocket,(SOCKADDR*)&clntAddr,&sizeClnt);
    if(clntSocket == INVALID_SOCKET) ErrorHandling(AcceptError);


    char message[] = "the day is to learn Chapter2!";

    // if(send(clntSocket,message,sizeof(message),0) == SOCKET_ERROR) ErrorHandling(SendError);

    //修改为延迟发送
    int eachLen = 0;
    int accLen = 0;
    int index = 0;
    while(eachLen = send(clntSocket,&message[index++],1,0))
    {
        if(eachLen == SOCKET_ERROR) ErrorHandling(SendError);


        if(message[index - 1] == '\0') break;
    }

    closesocket(clntSocket);
    closesocket(servSocket);
    WSACleanup();

    return 0;
}