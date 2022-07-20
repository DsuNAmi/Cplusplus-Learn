#include <iostream>
#include <winsock2.h>
#include <fstream>

#include "fileError.h"

using namespace std;

const char * fileName = "D:/Learn/C++Learn/workspace/project/TCPFileTrans/waitForTrans.txt";
const char * downloadName = "download.txt";



int main()
{
    WSADATA wsadata;
    SOCKET sock;
    SOCKADDR_IN addr;
    // FILE * source;

    if(WSAStartup(MAKEWORD(2,2),&wsadata)) ErrorHandling(StartUpError);

    sock = socket(PF_INET,SOCK_STREAM,0);
    if(sock == INVALID_SOCKET) ErrorHandling(SocketError);

    memset(&addr,0,sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ADDRESS);
    addr.sin_port = htons(atoi(PORT));

    if(connect(sock,(SOCKADDR*)&addr,sizeof(addr)) == SOCKET_ERROR) ErrorHandling(ConnectError);

    //sendFileName
    int sl = send(sock,fileName,strlen(fileName),0);
    if(sl == SOCKET_ERROR) ErrorHandling(SendError);

    //recvFile
    char fileBuff[BUFFSIZE];
    // string fileBuff;
    // source = fopen(downloadName,"wb");
    ofstream fout(downloadName,ios::out);

    if(!fout.is_open())
    {
        cout << "file create failed\n";
        closesocket(sock);
        WSACleanup();
        std::abort();
    }

    while((sl = recv(sock,fileBuff,BUFFSIZE,0)) > 0)
    {
        fout << fileBuff;
        fout << ' ';
    }


    fout.close();
    cout << "recv Over!\n";

    const char * message = "Thank you!";
    // shutdown(sock,SD_RECEIVE);
    sl = send(sock,message,strlen(message),0);
    closesocket(sock);
    WSACleanup();


    return 0;
}
