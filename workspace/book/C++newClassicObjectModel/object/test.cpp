#include <iostream>

// class A
// {
//     public:
//         void func() {};
//         void func1() {};
//         void func2() {};
//     public:
//         int ab;
// };

// class B
// {
//     static int a;
//     static int b;

// };

// class C
// {
//     static void sfunc();
//     void myfunc() {};
// };

// class D
// {
//     virtual void myfunc3() {};
//     virtual void myfunc4() {};
//     virtual void myfunc5() {};
// };


class A
{
    public:
        int a;
        A()
        {
            printf("A::A()'s this pointer is : %p!\n", this);
        }
        void funcA(){printf("A funcA:this = %p\n", this);}
};

class B
{
    public:
        int b;
        B()
        {
            printf("B::B()'s this pointer is : %p!\n", this);
        }
        void funcB(){printf("B funcB:this = %p\n", this);}
};


class C : public A, public B
{
    public:
        int c;
        C()
        {
            printf("C::C()'s this pointer is : %p!\n",this);
        }
        void funcC(){printf("C funcC this = %p\n",this);}
};


int main()
{
    std::cout << sizeof(A) << std::endl;
    std::cout << sizeof(B) << std::endl;
    std::cout << sizeof(C) << std::endl;

    C myc;
    myc.funcA();
    myc.funcB();
    myc.funcC();

    // A a;
    // B b;
    // C c;
    // D d;
    // int ilen = sizeof(a);
    // int blen = sizeof(b);
    // int clen = sizeof(c);
    // int dlen = sizeof(d);    
    // printf("ilen = %d\n", ilen);
    // printf("ilen = %d\n", blen);
    // printf("ilen = %d\n", clen);
    // printf("ilen = %d\n", dlen);

    return 0;
}