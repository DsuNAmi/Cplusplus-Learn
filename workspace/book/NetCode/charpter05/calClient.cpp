#include <iostream>
#include <winsock2.h>

#include "error4.h"


using namespace std;


const int BUFFSIZE = 1024;

int main(int argc, char * argv[])
{
    WSADATA wsadata;
    SOCKET clientSocket;
    SOCKADDR_IN address;

    if(argc != 3) ParamsFormat(argv[0],CLIENT);

    if(WSAStartup(MAKEWORD(2,2),&wsadata)) ErrorHandling(StartUpError);

    clientSocket = socket(PF_INET,SOCK_STREAM,0);
    if(clientSocket == INVALID_SOCKET) ErrorHandling(SocketError);

    memset(&address,0,sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(argv[1]);
    address.sin_port = htons(atoi(argv[2]));

    if(connect(clientSocket,(SOCKADDR*)&address,sizeof(address)) == SOCKET_ERROR) ErrorHandling(ConnectError);
    else printf("connecting....");

    cout << "Please Enter the numbers of operators: ";
    int operNumber = 0;
    cin >> operNumber;
    string message;
    for(int i = 0; i < operNumber; ++i)
    {
        cout << "the " << i + 1 << "th number: ";
        string number;
        cin >> number;
        message += number + "_";
    }

    cout << "The operator: ";
    string operatorS;
    cin >> operatorS;
    message += operatorS;

    // cout << "Wait to Send: " << message << endl;
    
    char * cMessage = const_cast<char*>(message.c_str());

    int sendLen = send(clientSocket,cMessage,strlen(cMessage),0);
    if(sendLen == SOCKET_ERROR) ErrorHandling(SendError);
    char result[BUFFSIZE];
    int recvLen = recv(clientSocket,result,BUFFSIZE - 1,0);
    if(recvLen == SOCKET_ERROR) ErrorHandling(RecvError);
    
    cout << "result from server: " << result << endl;

    closesocket(clientSocket);
    WSACleanup();

    return 0;
}