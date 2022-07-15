#include "Bird.h"




Bird::Bird(int x, int y, const std::vector<std::vector<char>> & backGround) 
: uppperBound(0),bottomBound(MAXBOUNDROW),llefttBound(0),righttBound(MAXBOUNDCOL),
  birdFace('#')
{
    this->SetBirdPosition(x,y,backGround);
}

bool Bird::SetBirdPosition(int x, int y, const std::vector<std::vector<char>> & backGround)
{
    if(x <= llefttBound || x >= righttBound - 1 || y <= uppperBound || y >= bottomBound - 1) return false;
    if(IsCollision(backGround)) return false;
    this->xCoord = x;
    this->yCoord = y;
    return true;
}

bool Bird::IsCollision(const std::vector<std::vector<char>> & backGround) const
{
    if((backGround[this->yCoord][this->xCoord] == P + this->birdFace) || 
       (backGround[this->yCoord][this->xCoord] == L + this->birdFace) ||
       (backGround[this->yCoord][this->xCoord] == R + this->birdFace) ||
       (backGround[this->yCoord][this->xCoord] == U + this->birdFace) ||
       (backGround[this->yCoord][this->xCoord] == B + this->birdFace)) return true;
    return false;
}


bool Bird::MoveUp()
{
    if(this->yCoord <= this->uppperBound + 2) return false;
    --(this->yCoord);
    return true;
}

bool Bird::MoveDown()
{
    if(this->yCoord >= this->bottomBound - 2) return false;
    ++(this->yCoord);
    return true;
}

bool Bird::MoveLeft()
{
    if(this->xCoord <= this->llefttBound + 2) return false;
    --(this->xCoord);
    return true;
}

bool Bird::MoveRight()
{
    if(this->xCoord >= this->righttBound - 2) return false;
    ++(this->xCoord);
    return true;
}