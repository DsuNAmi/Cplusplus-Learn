/* 
Author : DsuNAmi

projectName : Flappy Bird Easy Verion

*/

#include <iostream>
#include <ctime>
#include <windows.h>

#include "Bird.h"
#include "background.h"

#define Clear() printf("\033[2K")

int main()
{
    // printf("Do the Flappy Bird!");

    /* 初始化 */
    BackGround bg;
    Bird flappyBird;
    srand(std::time(0));
    int startY = rand() % bg.GetBBound();
    while(!flappyBird.SetBirdPosition(1,startY,bg.GETBG()))
    {
        startY = rand() % bg.GetBBound();
    }
    bg.GenerBird(flappyBird.GetXCoord(),flappyBird.GetYCoord(),flappyBird.GetFace());
    bg.GenerUI();
    //设定飞往的目标
    flappyBird.GetCurTarget(bg.GETBG(),bg.GetPillars());

    /* 主程序循环 */

    
    int TestTimes = 200;

    while(TestTimes)
    {
        --TestTimes;
        bg.ShuffBird(flappyBird.GetXCoord(),flappyBird.GetYCoord());
        // flappyBird.MoveDown();
        // flappyBird.MoveRight();
        /* 自动判断移动位置 */
        flappyBird.IsArrive(bg.GETBG(),bg.GetPillars());
        flappyBird.AutoFly(bg.GETBG(),bg.GetPillars());
        bg.MovePillars();
        bg.GenerBird(flappyBird.GetXCoord(),flappyBird.GetYCoord(),flappyBird.GetFace());
        if(flappyBird.IsCollision(bg.GETBG()))
        {
            printf("Game Over!");
            std::abort();
        }
        Clear();
        bg.GenerUI();
        Sleep(500); //1-second
    }

    return 0;
}