#include <iostream>
#include <vector>

using namespace std;


vector<bool> Test(const vector<bool> & t)
{
    return t;
}


int main()
{
    vector<bool> test(5);
    auto x = static_cast<bool>(Test(test)[3]);
    cout << x << endl;
    return 0;
}