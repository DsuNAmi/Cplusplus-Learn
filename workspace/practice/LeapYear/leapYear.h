/**
 * @file leapYear.cpp
 * @author DsuNAmi (DsuNAmi@163.com)
 * @brief 判断是否是闰年的类
 * @version 0.1
 * @date 2022-07-07
 * 
 * @copyright Copyright (c) 2022
 * 
 */


#ifndef __LEAPYEAR__
#define __LEAPYEAR__

#include <ctime>

class LeapYear
{
private:
    int year;
    bool isLeapYear;
    bool JudgeLeapYear();
public:
    LeapYear(){
        time_t t = time(NULL);
        tm * pt = localtime(&t);
        year = pt->tm_year + 1900;
        isLeapYear = JudgeLeapYear();
    };
    LeapYear(int year)
    {
        this->year = year;
        isLeapYear = JudgeLeapYear();
    }

    bool GetLeap() {return this->isLeapYear;}

    ~LeapYear() {}
};

bool LeapYear::JudgeLeapYear()
{
    return (((this->year % 4 == 0) && (this->year % 100 != 0)) || (this->year % 400 == 0));
}

#endif