#include "Bird.h"




Bird::Bird(int x, int y, const std::vector<std::vector<char>> & backGround) 
: uppperBound(0),bottomBound(MAXBOUNDROW),llefttBound(0),righttBound(MAXBOUNDCOL),
  birdFace('#'),
  xTarget(1),
  yTarget(0)
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

void Bird::GetCurTarget(const std::vector<std::vector<char>> & backGround, const int * pillars)
{
    /* 
    
    curu, curd, curNextu, curNextd
    1.
    
    */

    int cur = this->xTarget;
    int next = (cur + 1) % INITPILLARS;
    int curu = this->uppperBound + 1, curd = this->bottomBound - 2;
    int nextu = this->uppperBound + 1, nextd = this->bottomBound - 2;
    for(int ui = uppperBound + 1; ui <= this->bottomBound - 2; ++ui)
    {
        if(backGround[ui][pillars[cur]] == ' ') {curu = ui; break;}
    }
    for(int ui = uppperBound + 1; ui <= this->bottomBound - 2; ++ui)
    {
        if(backGround[ui][pillars[next]] == ' ') {nextu = ui; break;}
    }
    for(int ui = this->bottomBound - 2; ui >= uppperBound + 1; --ui)
    {
        if(backGround[ui][pillars[cur]] == ' ') {curd = ui; break;}
    }
    for(int ui = this->bottomBound - 2; ui >= uppperBound + 1; --ui)
    {
        if(backGround[ui][pillars[next]] == ' ') {nextd = ui; break;}
    }

    this->yTarget = curu;
    if(nextu < curd) this->yTarget = curd;
}

void Bird::AutoFly(const std::vector<std::vector<char>> & backGround,const int * pillars)
{
    if(this->xCoord < pillars[this->xTarget])
    {
        if(this->yCoord < this->yTarget)
        {
            this->MoveDown();
        }
        else if(this->yCoord > this->yTarget)
        {
            this->MoveUp();
        }
    }
}

bool Bird::IsArrive(const std::vector<std::vector<char>> & backGround,const int * pillars)
{
    if(this->xCoord == pillars[this->xTarget])
    {
        //更新目标
        this->xTarget = (this->xTarget + 1) % INITPILLARS;
        this->GetCurTarget(backGround,pillars);
        return true;
    }
    //doNothing
    return false;
}

