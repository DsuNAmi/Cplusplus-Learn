#include <iostream>
#include <winsock2.h>

#include "error.h"

using namespace std;


void ErrorHandling(const char * message);


int main(int argc, char * argv[])
{
    WSADATA wsaData;
    SOCKET servSocket, clientSocket;
    sockaddr_in servAddr, clientAddr;

    if(argc != 2)
    {
        ParamsFormat(argv[0],SERVER);
    }


    if(WSAStartup(MAKEWORD(2,2),&wsaData)) ErrorHandling(StartUpError);

    servSocket = socket(PF_INET,SOCK_STREAM,0);
    if(servSocket == INVALID_SOCKET) ErrorHandling(SocketError);

    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(atoi(argv[1]));

    if(bind(servSocket,(SOCKADDR*)&servAddr,sizeof(servAddr)) == SOCKET_ERROR) ErrorHandling(BindError);

    if(listen(servSocket,6) == SOCKET_ERROR) ErrorHandling(ListenError);

    int sizeClientAddr = sizeof(clientAddr);
    clientSocket = accept(servSocket,(SOCKADDR*)&clientAddr,&sizeClientAddr);
    if(clientSocket == INVALID_SOCKET) ErrorHandling(AcceptError);

    char message[] = "This is New Hello World!";
    if(send(clientSocket,message,sizeof(message),0) == SOCKET_ERROR) ErrorHandling(SendError);

    closesocket(clientSocket);
    closesocket(servSocket);
    WSACleanup();

    return 0;
}


void ErrorHandling(const char * message)
{
    fputs(message, stderr);
    fputc('\n',stderr);
    exit(1);
}