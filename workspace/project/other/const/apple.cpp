#include <iostream>

using namespace std;

class Apple
{
    private:
        int people[100];
    public:
        Apple(int i);
        const int apple_number;
        void take(int num) const;
        int add();
        int add(int num) const;
        int getCount() const;
};

Apple::Apple(int i):apple_number(i)
{

}

int Apple::add()
{
    take(1);
    return 0;
}

int Apple::add(int num) const
{
    take(num);
    return num;
}

void Apple::take(int num) const
{
    cout << "take func " << num << endl; 
}

int Apple::getCount() const
{
    take(1);
    add(); //error
    return apple_number;
}

int main()
{
    Apple a(2);
    cout << a.getCount() << endl;
    a.add(10);
    return 0;
}
