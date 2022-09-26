#include <iostream>
#include <ctime>
#include <chrono>
#include <memory>

// class Widget {};

// void process(const Widget & lvalArg);           //处理左值
// void process(Widget && rvalArg);                //处理右值


// template<typename T>
// void logAndProcess(T && param)
// {
//     auto now = std::chrono::system_clock::now();

//     process(std::forward<T>(param));
// }

// //考虑两种调用情形，一种向其传入左值，一种向其传入右值
// Widget w;
// LogAndprocess(w);                     //左值
// LogAndprocess(std::move(w));          //右值



// class Widget
// {
//     private:
//         static std::size_t moveCtroCalls;
//         std::string s;

//     public:
//         Widget(Widget && rhs) : s(std::move(rhs.s)) {++moveCtroCalls;}

// };


// class Widget
// {
//     private:
//         std::string name;
//         std::shared_ptr<int> p;
//     public:
//         Widget(Widget && rhs) : name(std::move(rhs.name)), p(std::move(rhs.p)) {}


// };


// class Widget
// {
//     public:
//     template<typename T>
//     void setName(T && newName)
//     {
//         name = std::forward<T>(newName);
//     }
// };

// class Widget
// {
//     public:
//         template<typename T>
//         void setName(T && newName)
//         {
//             name = std::move(newName);
//         }
//     private:
//         std::string name;
//         std::shared_ptr<int> P;
// };



int main()
{
    int x = 8;
    int && y = std::move(x);
    printf("%d",y);
    printf("%d",x);

    std::false_type test;
    std::cout << test << std::endl;

    return 0;
}