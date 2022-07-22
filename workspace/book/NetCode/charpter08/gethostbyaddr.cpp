#include <iostream>
#include <winsock2.h>

#include "error7.h"

using namespace std;


int main()
{
    WSADATA wsadata;
    hostent * host;
    SOCKADDR_IN addr;
    

    if(WSAStartup(MAKEWORD(2,2),&wsadata)) ErrorHandling(StartUpError);

    memset(&addr, 0, sizeof(addr));
    addr.sin_addr.s_addr = inet_addr(CHANGEADDRESS);


    host = gethostbyaddr((char*)&addr.sin_addr,4,AF_INET);

    cout << "Official Name: " << host->h_name << endl;
    for(int i = 0; host->h_aliases[i]; ++i)
    {
        cout << "Aliases: " << host->h_aliases[i] << endl; 
    }
    cout << "Address type: " << (host->h_addrtype == AF_INET) ? "AF_INET" : "AF_INET6";
    cout << endl;
    for(int i = 0; host->h_addr_list[i]; ++i)
    {
        cout << "IP address: " << inet_ntoa(*(in_addr*)host->h_addr_list[i]) << endl;
    }

    WSACleanup();

    return 0;
}