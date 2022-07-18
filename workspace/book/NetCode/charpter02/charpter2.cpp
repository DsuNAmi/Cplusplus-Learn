#include <iostream>
#include <winsock2.h>

#include "error2.h"

using namespace std;

int main()
{
    WSADATA wsadata;
    SOCKET servSocket;

    if(WSAStartup(MAKEWORD(2,2),&wsadata)) ErrorHandling(StartUpError);

    // servSocket = socket()

    return 0;
}