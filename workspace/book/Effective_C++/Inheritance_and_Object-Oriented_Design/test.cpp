#include <iostream>

using namespace std;

class Base
{
    public:
        virtual void mf1() {cout << "Im Base";}
        virtual void mf1(int) {cout << "mf2";}
};

class Derived : private Base
{    public:
        virtual void mf1() {cout << "fdaf"; Base::mf1();}        //转交函数
};


class Time
{
    public:
        double time;
        Time() : time(0) {}
        virtual void GoTime() = 0;
        virtual void Notime() {cout << "NoTime Now";}
};

void Time::GoTime()
{
    cout << "Im pure virtual";
}

class subTime : public Time
{
    public:
        void GoTime() {cout << "Time goes on...";}
        double GetTime() {return this->time;}
        void Notime() {cout << "Now No Time";}
};

int main()
{
    Time * t = new subTime();
    t->Time::GoTime();
    t->Notime();
    subTime st;
    cout << st.GetTime() << endl;
    return 0;
}