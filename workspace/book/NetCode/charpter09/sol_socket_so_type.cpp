#include <iostream>
#include <winsock2.h>

using namespace std;


void ErrorHanding(const char * message)
{
    cout << message << endl;
    abort();
}


int main()
{
    WSADATA wsadata;
    int tcpSock;
    int udpSock;
    int sockType;
    int optlen;
    int state;

    if(WSAStartup(MAKEWORD(2,2),&wsadata)) ErrorHanding("StartUp Error!");

    optlen = sizeof(sockType);
    tcpSock = socket(PF_INET,SOCK_STREAM,0);
    udpSock = socket(PF_INET,SOCK_DGRAM,0);

    cout << "SOCK_STREAM : " << SOCK_STREAM << endl;
    cout << "SOCK_DGRAM  : " << SOCK_DGRAM  << endl;


    state = getsockopt(tcpSock,SOL_SOCKET,SO_TYPE,(char*)&sockType,&optlen);
    if(state) ErrorHanding("getsockopt() error!");
    cout << "Socket type one : " << sockType << endl;
    
    state = getsockopt(udpSock,SOL_SOCKET,SO_TYPE,(char*)&sockType,&optlen);
    if(state) ErrorHanding("getsockopt() error!");
    cout << "Socket type two : " << sockType << endl;


    //注意，套接字只能在创建时决定，以后不能更改。
    //SO_TYPE是典型的只读选项

    return 0;
}