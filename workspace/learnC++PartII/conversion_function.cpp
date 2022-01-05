#include <iostream>

using namespace std;

class Fraction
{
    public:
    	Fraction(int num, int den = 1)
            :m_numerator(num), m_denominator(den) {}
    operator double() const { //转换函数不可以有参数，也没有返回值，通常都会加上const
        return ((double)m_numerator / (double)m_denominator);
    }
    private:
    	int m_numerator;//分子
    	int m_denominator;//分母
};


int main()
{
    Fraction f(3,5);
    double d = 4 + f;
    cout << d << endl;
    return 0;
}