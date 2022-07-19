#include <iostream>
#include <vector>


// class Person
// {
//     public:
//         Person();
//         virtual ~Person();
//     private:
//         std::string name;
//         std::string address;
// };


// class Student : public Person
// {
//     public:
//         Student();
//         ~Student();
//     private:
//         std::string schoolName;
//         std::string schoolAddress;
// };

// class Window
// {
//     public:
//         virtual void display() const {cout << "I am Base Window\n"; }
    
// };

// class WindoWithScrollBars : public Window
// {
//     public:
//         virtual void display() const {cout << "I am Window with ScrollBars\n";}
// };


// void print(const Window & w)
// {
//     w.display();
//     return;
// }



// namespace std
// {
//     template<>
//     void swap<Widget>(Widget & a, Widget & b) //这是std::swap针对T是Widget的特化版本。
//     {
//         a.swap(b.pImpl);			//私有变量，还不能通过编译。
//     }
// }


// class WidgetImpl
// {

    	
//     private:
//     	int a, b, c;				//可能有很多数据
//     	std::vector<double> v;		//因为复制很长时间
    	
// };




// class Widget			//pimpl
// {
//     public:
// 		Widget(const Widget & rhs);
//     	Widget & operator=(const Widget & rhs)
//         {
//             *pImpl = (*rhs.pImpl);
// 		}
//         void swap(Widget & other)
//         {
//             using std::swap;
//             swap(pImpl,other.pImpl);
//         }
//     private:
//     	WidgetImpl * pImpl;
// };


int main()
{
    int a = 3;
    int b = 4;
    return 0;
}