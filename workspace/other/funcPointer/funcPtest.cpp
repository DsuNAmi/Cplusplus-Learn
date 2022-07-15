#include <iostream>

double betsy(int);
double pam(int);


void estimate(int lines, double (*pf)(int));



void f1(int * );
const char * f2(const int * , const int *);


typedef void (*FunOne)(int *);
typedef const char * (*FunTwo)(const int *, const int *);

int main()
{
    using namespace std;
    FunOne p1 = f1;
    FunTwo p2 = f2;

    FunOne ap[5];
    FunTwo pa[10];

    for(int i = 0; i < 5; ++i)
    {
        ap[i] = f1;
    }
    for(int i = 0; i < 10; ++i)
    {
        pa[i] = f2;
    }

    int a = 3;
    int a1 = 1;
    int a2 = 1;

    p1(&a);
    cout << p2(&a1,&a2) << endl;
    
    for(int i = 0; i < 5; ++i)
    {
        ap[i](&a);
    }
    for(int i = 0; i < 10; ++i) pa[i](&a1,&a2);

    return 0;
}

double betsy(int lns) {return 0.05 * lns;}

double pam(int lns) {return 0.03 * lns + 0.0004 * lns * lns;}

void estimate(int lines, double (*pf)(int))
{
    using namespace std;
    cout << lines << " lines will take ";
    cout << (*pf)(lines) << " hour(s)\n";    
}

void f1(int * a)
{
    std::cout << *a << std::endl;
}

const char * f2(const int * a1, const int * a2)
{
    std::cout << (int)(*a1 + *a2) << std::endl;
    return "f2";
}