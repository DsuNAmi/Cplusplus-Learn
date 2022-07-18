#include <iostream>
#include <winsock2.h>

#include "error3.h"


using namespace std;

const int BUFFSIZE = 1024;


int main(int argc, char * argv[])
{
    WSADATA wsadata;
    SOCKET clientSocket;
    SOCKADDR_IN servAddr;


    if(argc != 3) ParamsFormat(argv[0],CLIENT);

    if(WSAStartup(MAKEWORD(2,2),&wsadata)) ErrorHandling(StartUpError);

    clientSocket = socket(PF_INET,SOCK_STREAM,0);
    if(clientSocket == INVALID_SOCKET) ErrorHandling(SocketError);

    memset(&servAddr,0,sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = inet_addr(argv[1]);
    servAddr.sin_port = htons(atoi(argv[2]));

    if(connect(clientSocket,(SOCKADDR*)&servAddr,sizeof(servAddr))) ErrorHandling(ConnectError);
    else
    {
        //因为连接了并不是直接进行数据交换，而是放入服务器的等待队列
        cout << "Connceting...." << endl;
    }

    while(1)
    {
        char message[BUFFSIZE];
        int acceptLen = 0;
        fputs("Input message(Q TO quit): ",stdout);
        fgets(message,BUFFSIZE,stdin);

        if(!strcmp(message,"q\n") || !strcmp(message,"Q\n")) break;
        send(clientSocket,message,strlen(message), 0);
        acceptLen = recv(clientSocket,message,BUFFSIZE - 1,0);
        message[acceptLen] = 0;
        printf("Echo message: %s",message);
    }

    closesocket(clientSocket);
    WSACleanup();
    return 0;
}