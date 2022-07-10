#include <iostream>

using namespace std;

int f()
{
    int * p = (int *)malloc(sizeof(int));
    *p = 10;
    cout << *p << endl;

#ifndef DEBUG
    int error = 1;
#endif
    if(error)
        goto END;
    //dosomething
END:
    cout << "free" << endl;
    free(p);
    return 0;
}


int ff()
{
    int * p = (int *)malloc(sizeof(int));
    *p = 10;
    cout << *p << endl;

    do{
        #ifndef DEBUG
            int error = 1;
        #endif
            if(error) break;
        //dosomething
    }while(0);
    cout << "free" << endl;
    free(p);
    return 0;
}


int main()
{
    return 0;
}