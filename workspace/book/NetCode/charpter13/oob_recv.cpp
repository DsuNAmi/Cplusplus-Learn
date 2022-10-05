#include <iostream>
#include <winsock2.h>

const int BUFFSIZE = 30;
void ErrorHandling(char * message);

const char * PORT = "8080";


int main()
{
    WSADATA wsadata;
    SOCKET hAcptSock, hRecvSock;
    SOCKADDR_IN recvAdr;
    SOCKADDR_IN sendAdr;
    char buf[BUFFSIZE];
    int result;
    int sendAdrSize;
    int strlen;

    fd_set read, except, readCopy, exceptCopy;
    timeval timeout;

    WSAStartup(MAKEWORD(2,2),&wsadata);

    hAcptSock = socket(PF_INET, SOCK_STREAM, 0);
    memset(&recvAdr, 0, sizeof(recvAdr));
    recvAdr.sin_family = AF_INET;
    recvAdr.sin_addr.s_addr = htonl(INADDR_ANY);
    recvAdr.sin_port = htons(atoi(PORT));

    bind(hAcptSock,(SOCKADDR*)&recvAdr,sizeof(recvAdr));

    listen(hAcptSock,5);

    sendAdrSize = sizeof(sendAdr);

    hRecvSock = accept(hAcptSock,(SOCKADDR*)&sendAdr, &sendAdrSize);

    FD_ZERO(&read);
    FD_ZERO(&except);
    FD_SET(hRecvSock, &read);
    FD_SET(hRecvSock, &except);

    while(1)
    {
        readCopy = read;
        exceptCopy = except;
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;

        result = select(0, &readCopy, 0, &exceptCopy, & timeout);

        if(result > 0)
        {
            if(FD_ISSET(hRecvSock, &exceptCopy))
            {
                strlen = recv(hRecvSock, buf, BUFFSIZE - 1, MSG_OOB);
                buf[strlen] = 0;
                printf("Urgent message : %s \n", buf);
            }

            if(FD_ISSET(hRecvSock, &readCopy))
            {
                strlen = recv(hRecvSock, buf, BUFFSIZE - 1, 0);
                if(strlen == 0)
                {
                    break;
                    closesocket(hRecvSock);
                }
                else
                {
                    buf[strlen] = 0;
                    puts(buf);
                }
                
            }
        }
    }


    closesocket(hAcptSock);
    WSACleanup();
    return 0;
}

void ErrorHanling(char * message)
{
    fputs(message, stderr);
    fputc('\n',stderr);
    exit(1);
}