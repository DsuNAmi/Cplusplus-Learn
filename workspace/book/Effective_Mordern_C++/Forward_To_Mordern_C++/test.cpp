#include <iostream>
#include <vector>
#include <tuple>
#include <algorithm>
#include <mutex>
#include <atomic>
#include <cmath>

using namespace std;


// using userInfo = std::tuple<std::string,        //name
//                             std::string,        //email
//                             std::size_t>;       //reputation



// class Test
// {
//     public:
//         Test() = delete;
//         Test(const Test & t) = delete;
//         Test& operator=(const Test & t) = delete;
// };

// bool isLucky(int number) {return true;}

// bool isLucky(bool) = delete;

// bool isLucky(double) = delete;

// bool isLucky(char) = delete;

// class Base
// {
//     public:
//         virtual void mf1() const;
//         virtual void mf2(int x);
//         virtual void mf3() &;
//         void mf4() const;
// };

// class Derived : public Base
// {
//     //派生类的函数真的能够动态编译吗？
//     public:
//         virtual void mf1();
//         virtual void mf2(unsigned int x);
//         virtual void mf3() &&;
//         void mf4() const;
// };

// //如果我们加上override
// class Derived : public Base
// {
//     //满足重写要求，没问题
//     public:
//         virtual void mf1() const override;
//         virtual void mf2(int x) override;
//         virtual void mf3() & override;
//         void mf4() const override;  //在基类加virtual
// };


// constexpr int pow(int base, int exp) noexcept       //pow是个constexpr函数，且不会抛出异常
// {
//     //C++11中，constexpr中不能包含超过一条语句
//     return (exp == 0 ? 1 : base * pow(base,exp - 1));
//     //C++14中没有这个限制
//     auto result  = 1;
//     for(int i = 0; i < exp; ++i) result *= base;
//     return result;
// }

// constexpr auto numConds = 5;

// std::array<int, pow(3,numConds)> results;       //results有3^numConds个元素


// class Point
// {
//     public:
//         constexpr Point(double xVal = 0, double yVal = 0) noexcept
//         :x(xVal), y(yVal) {}
        
//         constexpr double xValue() const noexcept {return x;}    //所以x,y都是编译期可知
//         constexpr double yValue() const noexcept {return y;}

//         constexpr void setX(double newX) noexcept {x = newX;}
//         constexpr void setY(double newY) noexcept {y = newY;}

//     private:
//         double x, y;
// };


// constexpr Point p1(9.4,27.4);
// constexpr Point p2(28.8,5.3);

// constexpr Point midpoint(const Point & p1, const Point & p2) noexcept
// {
//     return {(p1.xValue() + p2.xValue()) / 2,
//             (p1.yValue() + p2.yValue()) / 2};       //调用constexpr成员函数
    
// }

// constexpr auto mid = midpoint(p1,p2);

// //中心对称
// constexpr Point reflection(const Point & p) noexcept
// {
//     Point result;

//     result.setX(-p.xValue());
//     result.setY(-p.yValue());


//     return result;
// }

// constexpr auto reflectionMid = reflection(mid);


// class Polynomial
// {
//     public:
//         using RootType = std::vector<double>;

//         RootType roots() const
//         {
//             std::lock_guard<std::mutex> g(m);       //加上互斥量
//             if(!rootsAreValid)
//             {
//                 rootsAreValid = true;
//             }
//             return rootVals;
//         }

//     private:
//         mutable std::mutex m;
//         mutable bool rootsAreValid{false};
//         mutable RootType rootVals {};
        
// };


// Polynomial p;

// //在本例中，会发生 数据竞险，这段代码存在未定义行为。
// auto rootsOfP = p.roots();
// auto valsGivingZero = p.roots();


// class Point
// {
//     public:
//         double distanceFromOrigin() const noexcept
//         {
//             ++callCount;        //带原子性的自增操作

//             return sqrt((x * x) + (y * y));
//         }

//     private:
//         mutable std::atomic<unsigned> callCount {0};
//         double x, y;
// };


// class Widget
// {
//     public:
//         int magicValue() const
//         {
//             std::lock_guard<std::mutex> g(m);

//             if (cachedValid) return cachedValue;
//             else
//             {
//                 auto val1 = expensiveComputation1();
//                 auto val2 = expensiveComputation2();
//                 cachedValue = val1 + val2;
//                 cachedValid = true;
//                 return cachedValue;
//             }
//         }

//     private:
//         mutable std::mutex m;
//         mutable std::atomic<bool> cachedValid{false};
//         mutable std::atomic<int> cachedValue;
// };

// class Widget
// {
//     public:
//         ~Widget();

//         Widget(const Widget &) = default;       //默认复制构造函数的行为是正确的

//         Widget & operator=(const Widget &) = default; //同理
// };


class Base
{
    public:
        virtual ~Base();     //不让生成移动操作的条件不满足，
        Base(Base&&);            //提供移动操作的支持
        Base & operator=(Base&&);

        Base(const Base & );      //提供复制操作的支持
        Base & operator=(const Base &) = default;
};

int main()
{

    // Test t;
    // enum class Color {black, white, red};

    // auto white = false;

    // Color c = white;            //错误！范围内并无此枚举量

    // Color c = Color::white;     //可以

    // auto c = Color::white;      //可以

    // enum class Color {black, white, red};

    // std::vector<std::size_t> primeFactors(std::size_t);

    // Color c = Color::red;

    // if(c < 14.5)        //枚举和浮点数比较
    // {
    //     auto factors = primeFactors(c);     //
    // }


    // userInfo uInfo;

    // auto val = std::get<1>(uInfo);     //取用域1的值，但是我们有时候并不知道域1是什么，可以采用枚举

    // enum UserInfoFields {uiName, uiEamil, uiReputation};

    // auto val = std::get<uiEamil>(uInfo);        //这样就好多了

    // userInfo uInfo;

    // enum class UserInfoFields {uiName, uiEmail, uiReputation};

    // auto val = std::get<static_cast<std::size_t>(UserInfoFields::uiReputation)>(uInfo);
    
    // std::vector<int> values;

    // auto it = std::find(values.cbegin(),values.cend(),1983);
    
    // values.insert(it,1998);

    // int sz;         //非constexpr变量

    // constexpr auto arraySize1 = sz;         //错误！sz的值在编译期未知

    // std::array<int,sz> data1;               //错误！一样的问题

    // constexpr auto arraySize2 = 10;         //10是一个编译期常量

    // std::array<int, arraySize2> data2;      //没问题，arraySize2 是一个constexpr

    // const auto arraySize = sz;          //没问题

    // std::array<int, arraySize> data3;       //编译期未知

    return 0;
}