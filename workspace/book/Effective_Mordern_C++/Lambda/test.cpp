#include <iostream>
#include <vector>
#include <functional>


using FilterConatainer = std::vector<std::function<bool(int)>>;


int computeSomeValue1(){}
int computeSomeValue2(){}
int computeDivisor(){}



void addDivisorFilter(FilterConatainer & filters)
{
    //我们可能需要在运算期计算出除数，而不是把硬编码的5写入lambda表达式
    auto calc1 = computeSomeValue1();
    auto calc2 = computeSomeValue2();
    auto divisor = computeDivisor();

    filters.emplace_back(
        [&](int value){return value % divisor == 0;} //危险，对divisor的指针可能空悬
    );

    filters.emplace_back(
        [&divisor](int value){return value % divisor == 0;}
    );
}

int main()
{
    FilterConatainer filters;           //元素为删选器的容器

    filters.emplace_back(
        [](int value) {return value % 5 == 0;}
    );

    



    return 0;
}