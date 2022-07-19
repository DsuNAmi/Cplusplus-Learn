#include <iostream>
#include <winsock2.h>

#include "fileError.h"


using namespace std;



int main()
{
    WSADATA wsadata;
    SOCKET servSocket, clntSocket;
    SOCKADDR_IN servAddr, clntAddr;
    FILE * source;


    if(WSAStartup(MAKEWORD(2,2),&wsadata)) ErrorHandling(StartUpError);

    servSocket = socket(PF_INET,SOCK_STREAM,0);
    if(servSocket == INVALID_SOCKET) ErrorHandling(SocketError);

    memset(&servAddr,0,sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(atoi(PORT));

    if(bind(servSocket,(SOCKADDR*)&servAddr,sizeof(servAddr)) == SOCKET_ERROR) ErrorHandling(BindError);

    if(listen(servSocket,5) == SOCKET_ERROR) ErrorHandling(ListenError);


    int clntSize = sizeof(clntAddr);
    clntSocket = accept(servSocket,(SOCKADDR*)&clntAddr,&clntSize);
    if(clntSocket == INVALID_SOCKET) ErrorHandling(AcceptError);
    
    //recvFileName
    char fileName[BUFFSIZE];
    int rl = recv(clntSocket,fileName,BUFFSIZE,0);
    cout << "send file : " << fileName << endl;
    if(rl == SOCKET_ERROR) ErrorHandling(RecvError);
    else
    {
        //findFile
        source = fopen(fileName,"r");
        if(source == nullptr)
        {
            ErrorHandling("No such File");
        } 
        else
        {
            char fileBuff[1024];
            while(!feof(source))
            {
                fread(fileBuff,1,BUFFSIZE,source);
                if((rl = send(clntSocket,fileBuff,BUFFSIZE,0)) == SOCKET_ERROR) break;
            }
        }
        fclose(source);
        cout << "Send file Over!\n";
    }

    closesocket(clntSocket);
    closesocket(servSocket);
    WSACleanup();

    return 0;
}