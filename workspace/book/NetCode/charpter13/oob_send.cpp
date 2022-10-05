#include <iostream>
#include <winsock2.h>

const int BUFFSIZE = 30;

const char * ADDRESS = "127.0.0.1";
const char * PORT = "8080";
void ErrorHandling(char * message);




int main()
{
    WSADATA wsadata;
    SOCKET hSocket;
    SOCKADDR_IN sendAdr;

    WSAStartup(MAKEWORD(2,2),&wsadata);

    hSocket = socket(PF_INET, SOCK_STREAM, 0);
    memset(&sendAdr, 0, sizeof(sendAdr));
    sendAdr.sin_family = AF_INET;
    sendAdr.sin_addr.s_addr = inet_addr(ADDRESS);
    sendAdr.sin_port = htons(atoi(PORT));

    connect(hSocket,(SOCKADDR*)&sendAdr, sizeof(sendAdr));

    send(hSocket,"123",3,0);
    send(hSocket,"4",1,MSG_OOB);
    send(hSocket,"567",3,0);
    send(hSocket,"890",3,MSG_OOB);

    closesocket(hSocket);
    WSACleanup();
    return 0;
}

void ErrorHandling(char * message)
{
    fputs(message, stderr);
    fputc('\n',stderr);
    exit(1);
}