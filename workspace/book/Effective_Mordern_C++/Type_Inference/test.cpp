#include <iostream>

using namespace std;

// template<typename T, size_t N>
// constexpr size_t ArraySize(T (&)[N]) noexcept
// {
//     return N;
// }

// typedef int (*FunptrT)(int);

// int Noone(int val)
// {

// }


// template<typename Container, typename Index>
// auto authAndAccess(Container & c, Index i)
// {
//     authenticateUser();
//     return c[i];
// }

int main()
{
    // int keyVals[] = {1,2,3,4};
    // int mappedVals[ArraySize(keyVals)];

    // int (*FunPtr)(int) = Noone;

    // FunptrT fp = Noone;

    // fp(1);

    // int x1 = 27;
    // int x2(27);

    // int x3 = {27};
    // int x4{27};
    
    // const int i = 0;        //decltype(i) -> const int

    // decltype(i) j = 0;

    int x = 0;
    const int y = 3;

    std::cout << typeid(x).name() << std::endl;
    std::cout << typeid(y).name() << std::endl;


    return 0;
}

