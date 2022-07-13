/* 
Author : DsuNAmi

projectName : Flappy Bird Easy Verion

*/

#include <iostream>
#include "background.h"

#define Clear() printf("\033[2K")

int main()
{
    // printf("Do the Flappy Bird!");

    BackGround bg;
    bg.GenerUI();
    
    int TestTimes = 100;

    while(TestTimes)
    {
        --TestTimes;
        bg.MovePillars();
        Clear();
        bg.GenerUI();
    }


    return 0;
}