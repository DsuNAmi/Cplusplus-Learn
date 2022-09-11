#include <iostream>
#include <vector>

using namespace std;

using v = vector<int>;


int main()
{
    v v1(20,10);
    for(auto x : v1) cout << x << endl;
    return 0;
}