#include <iostream>
#include <winsock2.h>

#include "error5.h"


int main()
{
    WSADATA wsadata;
    SOCKET sock;
    SOCKADDR_IN addr, caddr;

    if(WSAStartup(MAKEWORD(2,2),&wsadata)) ErrorHandling(StartUpError);

    sock = socket(PF_INET,SOCK_DGRAM,0);
    if(sock == INVALID_SOCKET) ErrorHandling(SocketError);

    memset(&addr,0,sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ADDRESS);
    addr.sin_port = htons(atoi(PORT));

    int times = 3;
    while(times > 0)
    {
        char message[BUFFSIZE];
        fputs("Input Your Message(q/Q to quit): ",stdout);
        fgets(message,BUFFSIZE,stdin);

        if(!strcmp(message,"q\n") || !strcmp(message,"Q\n")) break;

        int sl = sendto(sock,message,strlen(message),0,(SOCKADDR*)&addr,sizeof(addr));
        if(sl == SOCKET_ERROR) ErrorHandling(SendError);
        int csize = sizeof(caddr);
        int rl = recvfrom(sock,message,BUFFSIZE,0,(SOCKADDR*)&caddr,&csize);
        if(rl == SOCKET_ERROR) ErrorHandling(RecvError);
        message[rl] = 0;
        printf("Message from Server: %s",message);
        --times;
    }

    closesocket(sock);
    WSACleanup();

    return 0;
}