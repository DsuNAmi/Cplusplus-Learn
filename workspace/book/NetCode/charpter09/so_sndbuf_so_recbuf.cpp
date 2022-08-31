#include <iostream>
#include <winsock2.h>

using namespace std;

void ErrorHanding(const char * message)
{
    cout << message << endl;
    abort();
}


/* 
SO_SNDBUF, SO_RCVBUF是输出缓冲区大小相关选项。同这两个选项既可以读取缓冲区的大小，也可以更改
*/


void GetBuf()
{
    WSADATA wsadata;

    
    if(WSAStartup(MAKEWORD(2,2),&wsadata)) ErrorHanding("Start Error!");

    SOCKET sock;
    int sndBuf;
    int rcvBuf;
    int state;
    int len;

    sock = socket(PF_INET,SOCK_STREAM,0);
    len = sizeof(sndBuf);
    state = getsockopt(sock,SOL_SOCKET,SO_SNDBUF,(char*)&sndBuf,&len);
    
    if(state) ErrorHanding("getsockopt() error!");
    len = sizeof(sndBuf);
    state = getsockopt(sock,SOL_SOCKET,SO_SNDBUF,(char*)&sndBuf,&len);
    
    if(state) ErrorHanding("getsockopt() error!");

    cout << "Input Buffer: " << sndBuf << endl;
    cout << "Output Buffer: " << rcvBuf << endl;
}

void SetBuf()
{
    WSADATA wsadata;

    
    if(WSAStartup(MAKEWORD(2,2),&wsadata)) ErrorHanding("Start Error!");

    SOCKET sock;
    int sndBuf = 1024 * 3;
    int rcvBuf = 1024 * 3;
    int state;
    int len;

    sock = socket(PF_INET, SOCK_STREAM, 0);
    state = setsockopt(sock,SOL_SOCKET,SO_RCVBUF,(char*)&rcvBuf,sizeof(rcvBuf));
    if(state) ErrorHanding("setsockopt() error");

    state = setsockopt(sock,SOL_SOCKET,SO_RCVBUF,(char*)&sndBuf,sizeof(sndBuf));
    if(state) ErrorHanding("setsockopt() error");

    len = sizeof(sndBuf);
    state = getsockopt(sock,SOL_SOCKET,SO_SNDBUF,(char*)&sndBuf,&len);
    
    if(state) ErrorHanding("getsockopt() error!");
    len = sizeof(sndBuf);
    state = getsockopt(sock,SOL_SOCKET,SO_SNDBUF,(char*)&sndBuf,&len);
    
    if(state) ErrorHanding("getsockopt() error!");


    //输出不会是设置的结果
    //并不会100%按照我们的请求设置
    cout << "Input Buffer: " << sndBuf << endl;
    cout << "Output Buffer: " << rcvBuf << endl;

}

int main()
{
    SetBuf();
    return 0;
}