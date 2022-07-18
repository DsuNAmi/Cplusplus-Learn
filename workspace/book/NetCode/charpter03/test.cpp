#include <iostream>
#include <winsock2.h>

using namespace std;



int main()
{
    unsigned short hp = 0x1234;
    unsigned short np;
    unsigned long ha = 0x12345678;
    unsigned long na;

    np = htons(hp);
    na = htonl(ha);

    printf("%#x",np);
    printf("%#lx",na);

    return 0;
}
