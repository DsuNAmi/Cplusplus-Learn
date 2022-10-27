#include <iostream>


#include <functional>


int echovalue(int value)
{
    std::cout << value << std::endl;
    return value;
}

template<typename T>
void myFunc(T && tmprv)
{
    //注意，&&是属于tmprv类型的一部分，不是T类型的一部分（&&和T类型没有关系）
    std::cout << tmprv << std::endl;
    return;
}


template<typename T>
class myTest
{
    public:
        void testfunc(T&& x) {}         //右值引用，因为不涉及推断，类创建的对象的时候已经确定了类型
        template<typename T2>
        void testfunc2(T2&&x){}         //万能引用，调用函数的时候还需要做推断
};

int main()
{
    std::function<int(int)> f1 = echovalue;
    f1(5);


    std::function<double(double)> f2 = [](double x)->double{
        std::cout << x << std::endl;
        return x;
    };

    f2(10);


    return 0;
}   