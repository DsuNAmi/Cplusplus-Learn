#ifndef __BIRD__
#define __BIRD__

#include <vector>

#include "background.h"

class Bird
{
    private:
        int xCoord;
        int yCoord;
        int uppperBound;
        int bottomBound;
        int llefttBound;
        int righttBound;
        //鸟的形象
        char birdFace;
        int xTarget;     //目标是几个柱子
        int yTarget;     //飞到的高度
    public:
        Bird() : xCoord(1), yCoord(1), 
                 uppperBound(0),bottomBound(MAXBOUNDROW),llefttBound(0),righttBound(MAXBOUNDCOL),
                 birdFace('#'),
                 xTarget(1),
                 yTarget(0)
                 {}
        Bird(int x, int y, const std::vector<std::vector<char>> & backGround);
        int GetUBound() const {return this->uppperBound;}
        int GetBBound() const {return this->bottomBound;}
        int GetLBound() const {return this->llefttBound;}
        int GetRBound() const {return this->righttBound;}
        int GetXCoord() const {return this->xCoord;}
        int GetYCoord() const {return this->yCoord;}
        char GetFace() const {return this->birdFace;}

        bool SetBirdPosition(int x, int y, const std::vector<std::vector<char>> & backGround);

        bool IsCollision(const std::vector<std::vector<char>> & backGround) const; //检测碰撞

        //移动
        bool MoveUp();
        bool MoveDown();
        bool MoveLeft(); 
        bool MoveRight();

        //获得当前飞往的目标
        void GetCurTarget(const std::vector<std::vector<char>> & backGround,const int * pillars);
        void AutoFly(const std::vector<std::vector<char>> & backGround,const int * pillars);
        bool IsArrive(const std::vector<std::vector<char>> & backGround,const int * pillars);
};


#endif