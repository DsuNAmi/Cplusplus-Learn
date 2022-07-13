#include <iostream>
#include <cstring>

using namespace std;

class Rational
{
    public:
        int num;
        double weight;
        Rational() {num = 0; weight = 0;}
        Rational(int num, double weight) : num(num), weight(weight) {}
};

// const Rational operator*(const Rational & lhs, const Rational & rhs)
// {
//     Rational temp;
//     temp.num = lhs.num * rhs.num;
//     temp.weight = lhs.weight * rhs.weight;
//     return temp;
// }

// class CTextBlock
// {
//     public:
//     	std::size_t length() const;
//     private:
//     	char * pText;
//     	mutable std::size_t textLength;			//最近一次计算的文本区块长度
//     	mutable bool lengthIsValid;				//目前的长度是否有效
// };

// std::size_t CTextBlock::length() const
// {
//     if(!lengthIsValid){
//         textLength = std::strlen(pText);
//         lengthIsValid = true;
//     }//错误！不能在const函数里面给两个变量赋值
//     return textLength;
// }

int main()
{
    // Rational a(1,2.0);
    // Rational b(2,4.0);
    // Rational c(1,3.0);

    // (a * b) = c;
    return 0;
}