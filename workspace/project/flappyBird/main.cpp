/* 
Author : DsuNAmi

projectName : Flappy Bird Easy Verion

*/

#include <iostream>

#include "Bird.h"
#include "background.h"

#define Clear() printf("\033[2K")

int main()
{
    // printf("Do the Flappy Bird!");

    BackGround bg;
    Bird flappyBird;
    bg.GenerBird(flappyBird.GetXCoord(),flappyBird.GetYCoord(),flappyBird.GetFace());
    bg.GenerUI();

    
    int TestTimes = 100;

    while(TestTimes)
    {
        --TestTimes;
        bg.ShuffBird(flappyBird.GetXCoord(),flappyBird.GetYCoord());
        bg.MovePillars();
        bg.GenerBird(flappyBird.GetXCoord(),flappyBird.GetYCoord(),flappyBird.GetFace());
        if(flappyBird.IsCollision(bg.GETBG()))
        {
            printf("Game Over!");
            std::abort();
        }
        Clear();
        bg.GenerUI();
    }

    return 0;
}