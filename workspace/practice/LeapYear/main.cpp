#include "leapYear.h"

#include <iostream>


int main()
{
    LeapYear thisYear;
    LeapYear lastYear(2020);
    std::cout << thisYear.GetLeap() << std::endl;
    std::cout << lastYear.GetLeap() << std::endl;
    return 0;
}