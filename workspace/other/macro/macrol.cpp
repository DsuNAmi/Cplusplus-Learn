#include <iostream>

using namespace std;


#define MAX(a,b) ((a) > (b) ? (a) \
    : (b))


int main()
{
    int maxVal = MAX(3,6);
    cout << maxVal << endl;
    return 0;
}