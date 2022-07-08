#include <iostream>

using namespace std;

class Apple
{
    public:
        //sattic member function
        static void printMsg()
        {
            cout << "Welcome to Apple!";
        }
};


int main()
{
    //invoking a static member function
    Apple::printMsg();
}