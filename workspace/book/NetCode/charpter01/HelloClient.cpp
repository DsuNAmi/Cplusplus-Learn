#include <iostream>
#include <winsock2.h>

#include "error.h"

using namespace std;

void ErrorHandling(const char * message);

int main(int argc, char * argv[])
{
    WSADATA wsaData;
    SOCKET clientSocket;
    sockaddr_in servAddr;

    char message[50];

    if(argc != 3) ParamsFormat(argv[0],CLIENT);

    if(WSAStartup(MAKEWORD(2,2),&wsaData)) ErrorHandling(StartUpError);

    clientSocket = socket(PF_INET,SOCK_STREAM,0);
    if(clientSocket == INVALID_SOCKET) ErrorHandling(SocketError);

    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = inet_addr(argv[1]);
    servAddr.sin_port = htons(atoi(argv[2]));

    if(connect(clientSocket,(SOCKADDR*)&servAddr,sizeof(servAddr)) == SOCKET_ERROR) ErrorHandling(ConnectError);

    int recvNumber;

    if(recv(clientSocket,message,sizeof(message) - 1,0) == SOCKET_ERROR) ErrorHandling(RecvError);

    printf("Recv from : %s", message);
    
    closesocket(clientSocket);
    WSACleanup();

    return 0;
}


void ErrorHandling(const char * message)
{
    fputs(message, stderr);
    fputc('\n',stderr);
    exit(1);
}