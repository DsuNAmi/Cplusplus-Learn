#include <iostream>
#include <winsock2.h>

#include "error2.h"

using namespace std;

int main(int argc, char * argv[])
{
    WSADATA wsadata;
    SOCKET clientSocket;
    sockaddr_in servAddr;


    if(argc != 3) ParamsFormat(argv[0],CLIENT);
    
    if(WSAStartup(MAKEWORD(2,2),&wsadata)) ErrorHandling(StartUpError);

    clientSocket = socket(PF_INET,SOCK_STREAM,0);
    if(clientSocket == INVALID_SOCKET) ErrorHandling(SocketError);

    memset(&servAddr,0,sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = inet_addr(argv[1]);
    servAddr.sin_port = htons(atoi(argv[2]));

    if(connect(clientSocket,(SOCKADDR*)&servAddr,sizeof(servAddr)) == SOCKET_ERROR) ErrorHandling(ConnectError);
    
    
    // int index = 0;
    // int eachLen = 0;
    char message[100];
    // int accLen = 0;
    // while(eachLen = recv(clientSocket,&message[index++],1,0))
    // {
    //     if(eachLen == SOCKET_ERROR) ErrorHandling(RecvError);

    //     accLen += eachLen;
    //     if(message[index- 1] == '\0') break;
        
    // }

    //一次接收
    for(int i = 0; i < 3000; ++i)
    {
        printf("Wait Time %d\n",i);
    }
    if(recv(clientSocket,message,sizeof(message) - 1,0) == SOCKET_ERROR) ErrorHandling(RecvError);

    printf("Recv from Server: %s\n",message);
    // printf("Recv Length: %d",accLen);

    closesocket(clientSocket);
    WSACleanup();
     
    return 0;
}