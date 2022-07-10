#include <iostream>
#include <vector>


using namespace std;

template <typename T>
auto multiply(T x, T y)->decltype(x * y)
{
    return x * y;
}

int main()
{
    int nums[] = {1,2,3,4};
    vector<int> vec(nums, nums + 4);
    vector<int>::iterator it;

    for(it = vec.begin(); it != vec.end(); ++it)
    {
        cout << *it << " ";
    }
    cout << endl;

    using nullptr_t = decltype(nullptr);

    nullptr_t nu;

    int * p = nullptr;
    if(p == nu) cout << "NULL" << endl;

    typedef decltype(vec.begin()) vectype;

    for(vectype i = vec.begin(); i != vec.end(); ++i)
    {
        cout << *i << " ";
    }
    cout << endl;

    struct
    {
        int d;
        double b;
    }anon_s;

    decltype(anon_s) as;

    cout << sizeof(multiply(11.0,2.2)) << endl;


    return 0;
}