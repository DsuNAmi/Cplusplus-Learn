#include <bits/stdc++.h>

using namespace std;


void Attributes()
{
    vector<int> attributes = vector<int>(13,0);
    for(int i = 0; i < 13; ++i)
    {
        attributes[i] = 1 + rand() % 20 ;
    }

    cout << "start Attributes" << endl;
    for(int i = 0; i < 13; ++i)
    {
        cout << i + 1 << ":" << attributes[i] << endl; 
    }
    cout << "end Attributes" << endl;
}


void CurrentAndFutures()
{
    cout << "c&f" << endl;
    int current = rand() % 11 + 90;
    int furture = rand() % 16 + 175;

    cout << "cur:" << current << endl;
    cout << "fur:" << furture << endl;
    cout << "end c&f " << endl;
}


void BuySomething()
{
    int even = 0;
    int odd = 0;
    for(int i = 0; i < 10; ++i)
    {
        int randomNumber = rand() % 21;
        if(randomNumber % 2) ++even;
        else ++odd;
    }

    if(even > odd) cout << "buy" << endl;
    else cout << "don't buy" << endl;
}


int main()
{
    srand(time(NULL));
    // Attributes();
    // CurrentAndFutures();
    BuySomething();
    return 0;
}