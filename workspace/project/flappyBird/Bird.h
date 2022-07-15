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
    public:
        Bird() : xCoord(1), yCoord(1), 
                 uppperBound(0),bottomBound(MAXBOUNDROW),llefttBound(0),righttBound(MAXBOUNDCOL),
                 birdFace('#')
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
};


#endif