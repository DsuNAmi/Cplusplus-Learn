#include <iostream>

using namespace std;

class Apple
{
    int i;
    public:
        Apple()
        {
            i = 0;
            cout << "Inside Constructor" << endl;
        }

        ~Apple()
        {
            cout << "Inside Destructor" << endl;
        }
};


int main()
{
    int x = 0;
    if(!x)
    {
        static Apple obj;
    }
    cout << "End of main" << endl;
}