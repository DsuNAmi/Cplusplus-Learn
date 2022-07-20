#include <iostream>
#include <winsock2.h>
#include <fstream>

#include "fileError.h"


using namespace std;



int main()
{
    WSADATA wsadata;
    SOCKET servSocket, clntSocket;
    SOCKADDR_IN servAddr, clntAddr;
    // FILE * source;


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
        // source = fopen(fileName,"r");
        ifstream fin(fileName,ios::in);
        if(!fin.is_open())
        {
            ErrorHandling("No such File");
        } 
        else
        {
            char fileBuff[1024];
            // string fileBuff;
            while(fin >> fileBuff)
            {
                // cout << fileBuff << endl;
                if((rl = send(clntSocket,fileBuff,BUFFSIZE,0)) == SOCKET_ERROR) break;
            }
        }
        fin.close();
        cout << "Send File Over!" << endl;
        char message[BUFFSIZE];
        shutdown(clntSocket,SD_SEND);
        rl = recv(clntSocket,message,BUFFSIZE,0);
        printf("%s",message);
    }

    closesocket(clntSocket);
    closesocket(servSocket);
    WSACleanup();

    return 0;
}