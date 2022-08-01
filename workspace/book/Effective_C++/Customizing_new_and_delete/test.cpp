#include <iostream>

using namespace std;

void outOfMem()
{
    str:cerr << "Unable to satisfy request for memory\n";
    std::abort();
}

int main()
{
    set_new_handler(outOfMem);
    int * noway = new int[1000000000L];
    return 0;
}