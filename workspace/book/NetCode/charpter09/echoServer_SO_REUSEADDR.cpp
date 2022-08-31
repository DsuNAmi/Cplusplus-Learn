#include <iostream>
#include <winsock2.h>

using namespace std;

int main()
{
    WSADATA wsadata;
    
    WSAStartup(MAKEWORD(2,2),&wsadata);

    SOCKET serverSock;
    SOCKET clientSock;
    SOCKADDR_IN serverAddr;
    SOCKADDR_IN clientAddr;
    int optlen;
    int clientAddrSize;
    int option;
    int strlen;
    char message[30];
    char * port = "8080";

    serverSock = socket(PF_INET,SOCK_STREAM,0);
    
    optlen = sizeof(option);
    option = 1;
    setsockopt(serverSock,SOL_SOCKET,SO_REUSEADDR,(char*)&option,optlen);

    memset(&serverAddr,0,sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(atoi(port));

    bind(serverSock,(SOCKADDR*)&serverAddr,sizeof(serverAddr));

    listen(serverSock,5);

    clientAddrSize = sizeof(clientAddr);
    clientSock = accept(serverSock,(SOCKADDR*)&clientAddr,&clientAddrSize);


    while(strlen = recv(clientSock,message,sizeof(message),0))
    {
        send(clientSock,message,strlen,0);
        send(1,message,strlen,0);//
    }

    closesocket(clientSock);
    closesocket(serverSock);


    return 0;
}