#include <iostream>
#include <string>
#include <cstring>
#include <winsock2.h>

using namespace std;

const int BUFFSIZE = 1024;
const char * ADDRESS = "127.0.0.1";
const char * PORT = "8080";

void ErrorHanding(char * message);


int main()
{
    WSADATA wsadata;
    SOCKET hServSock;
    SOCKET hClntSock;
    SOCKADDR_IN servAdr;
    SOCKADDR_IN clntAdr;
    TIMEVAL timeout;


    fd_set reads;
    fd_set cpyReads;

    int adrSz;
    int strlen;
    int fdNum;
    int i;
    char buf[BUFFSIZE];


    WSAStartup(MAKEWORD(2,2),&wsadata);

    hServSock = socket(PF_INET, SOCK_STREAM, 0);
    memset(&servAdr, 0, sizeof(servAdr));
    servAdr.sin_family = AF_INET;
    servAdr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAdr.sin_port = htons(atoi(PORT));

    bind(hServSock, (SOCKADDR*)&servAdr, sizeof(servAdr));

    listen(hServSock,5);

    FD_ZERO(&reads);            //给数据清零
    FD_SET(hServSock, &reads);  //注册要带读取的数据，这个地方会把sock的编号写到reads的数组里。


    while(1)
    {
        cpyReads = reads;       //复制一个副本，主要是变化的时候能够使用之前的版本
        timeout.tv_sec = 5;     //秒
        timeout.tv_usec = 5000; //毫秒

        fdNum = select(0, &cpyReads, 0, 0, &timeout);           //返回大于0的数，如果超时了就是返回0，因为没有客户端发请求，所以就选不到对应的号码。
        if(0 == fdNum) continue;        //如果是超时就是0，没有接收到数据
        
        for(int i = 0; i < reads.fd_count; ++i)     //这个就是更新其连接数量，如果有多个连接就是多个
        {
            if(FD_ISSET(reads.fd_array[i], &cpyReads))          //查看是否包含有信息
            {
                if(reads.fd_array[i] == hServSock)  //connection requeset
                {
                    adrSz = sizeof(clntAdr);
                    hClntSock = accept(hServSock, (SOCKADDR*)&clntAdr, &adrSz);     //接受客户端的连接请求
                    FD_SET(hClntSock, &reads); //重新注册新的位置
                    printf("connected client: %d \n", hClntSock);
                }
                else    //这个是接受消息的请求
                {
                    strlen = recv(reads.fd_array[i], buf, BUFFSIZE - 1, 0);     //接受发来的消息
                    if(strlen == 0)     //没有接受就
                    {
                        FD_CLR(reads.fd_array[i], &reads);          //没有接受就清空消息
                        closesocket(cpyReads.fd_array[i]);          //关闭与这个客户端的连接
                        printf("closed client : %d \n", cpyReads.fd_array[i]);
                    }
                    else
                    {
                        send(reads.fd_array[i],buf,strlen,0);       //回声
                    }
                }
            }
        }
    }

    closesocket(hServSock);
    WSACleanup();
    return 0;
}

void ErrorHanding(char * message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}