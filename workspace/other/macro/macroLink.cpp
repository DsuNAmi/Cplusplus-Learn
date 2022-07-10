#include <iostream>

using namespace std;

#define expA(s) printf("prefix added: %s \n", gc_##s) //gc_s 必须存在
#define expB(s) printf("prefix added: %s \n", gc_  ##  s) //gc_s必须存在
#define gc_hello1 "I am gc_hello1"

int main()
{
    const char * gc_hello = "I am gc_hello";
    expA(hello);
    expB(hello1);
    return 0;
}