#include <iostream>
#include <winsock2.h>

using namespace std;

int main()
{
    WSADATA wsaData;
    if(!WSAStartup(MAKEWORD(2,2),&wsaData))
    {
        cout << "WSAStartup error!" << endl;
    }

    
    return 0;
}