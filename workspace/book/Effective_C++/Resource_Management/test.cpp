#include <iostream>
#include <memory>

using namespace std;


class Investment {};


int main()
{
    auto_ptr<Investment> pInv(new Investment());

    if(pInv.get() == nullptr) cout << "pInv is nullptr1" << endl;

    auto_ptr<Investment> pInv2(pInv);

    if(pInv.get() == nullptr) cout << "pInv is nullptr2" << endl;

    pInv = pInv2;

    if(pInv.get() == nullptr) cout << "pInv is nullptr3" << endl;


    return 0;
}