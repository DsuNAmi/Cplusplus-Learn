#include <iostream>

using namespace std;


enum Weekday
{
    Monday,
    Tuesday,
    Wednesday,
    Thursday,
    Friday,
    Saturday,
    Sunday
};



int main()
{
    Weekday week = Monday;
    for(int i = week; i != Saturday; ++i)
    {
        cout << i << endl;
        cout << i + Monday << endl;
        cout << "------------" << endl;
    }

    return 0;
}