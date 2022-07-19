#include <iostream>
#include <winsock2.h>

#include "error4.h"

using namespace std;

const int BUFFSIZE = 1024;

int main(int argc, char * argv[])
{
    WSADATA wsadata;
    SOCKET servSocket, clntScoket;
    SOCKADDR_IN servAddr, clntAddr;

    if(argc != 2) ParamsFormat(argv[0],SERVER);

    if(WSAStartup(MAKEWORD(2,2),&wsadata)) ErrorHandling(StartUpError);

    servSocket = socket(PF_INET,SOCK_STREAM,0);
    if(servSocket == INVALID_SOCKET) ErrorHandling(SocketError);

    memset(&servAddr,0,sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(atoi(argv[1]));

    if(bind(servSocket,(SOCKADDR*)&servAddr,sizeof(servAddr)) == SOCKET_ERROR) ErrorHandling(BindError);

    if(listen(servSocket,5) == SOCKET_ERROR) ErrorHandling(ListenError);

    int clntSize = sizeof(clntAddr);
    
    clntScoket = accept(servSocket,(SOCKADDR*)&clntAddr,&clntSize);
    if(clntScoket == INVALID_SOCKET) ErrorHandling(AcceptError);

    char message[BUFFSIZE];

    int recvLen = recv(clntScoket,message,BUFFSIZE,0);
    if(recvLen == SOCKET_ERROR) ErrorHandling(RecvError);

    string oNumber = static_cast<string>(message);

    // cout << oNumber << endl;

    char oper = oNumber[(int)oNumber.size() - 1];
    // cout << oper << endl;
    int res = 0;
    if(oper == '+')
    {
        // cout << "cal" << endl;
        int number = 0;
        for(int i = 0; i < (int)oNumber.size(); ++i)
        {
            if(oNumber[i] == '_')
            {
                res += number;
                number = 0;
            }
            else
            {
                number = 10 * number + (oNumber[i] - '0');
            }
        }
    }
    else if(oper == '-')
    {
        int number = 0;
        for(int i = 0; i < (int)oNumber.size(); ++i)
        {
            if(oNumber[i] == '_')
            {
                res -= number;
                number = 0;
            }
            else
            {
                number = 10 * number + (oNumber[i] - '0');
            }
        }
    }
    else
    {
        int number = 0;
        for(int i = 0; i < (int)oNumber.size(); ++i)
        {
            if(oNumber[i] == '_')
            {
                res *= number;
                number = 0;
            }
            else
            {
                number = 10 * number + (oNumber[i] - '0');
            }
        }
    }

    cout << res << endl;

    char * result = const_cast<char *>(to_string(res).c_str());

    int sendLen = send(clntScoket,result,strlen(result),0);
    if(sendLen == SOCKET_ERROR) ErrorHandling(SendError);

    closesocket(clntScoket);
    closesocket(servSocket);
    WSACleanup();

    return 0;
}