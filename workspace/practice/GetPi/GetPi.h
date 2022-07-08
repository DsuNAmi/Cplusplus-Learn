#ifndef __GETPI__
#define __GETPI__


double GetPi();
double arctan(double x);


double GetPi()
{
    double a = 16.0 * arctan(1.0 / 5.0);
    double b = 4.0 * arctan(1.0 / 239.0);
    double pi = a - b;
    return pi;
}

double arctan(double x)
{
    double head = x;
    int tail = 1;
    double art = 0;
    while(double(head / tail) > 1e-15)
    {
        art= 1 == tail % 4 ? art + head / tail : art - head / tail;
        head *= x * x;
        tail += 2;
    }

    return art;
}

#endif