#include "ball.h"


Ball::Ball():
controlPlayer(nullptr),
hasPlayer(false)
{

}


Ball::Ball(Player * controlPlayer)
{
    if(!this->ChangeControlPlayer(controlPlayer))
    {
        this->hasPlayer = false;
    }
    else
    {
        this->hasPlayer = true;
    }
}

Ball::~Ball()
{
    //取消锁定
    //检查指针是否指着空对象
    this->controlPlayer = nullptr;
    delete this->controlPlayer;
}

bool Ball::ChangeControlPlayer(Player * otherControlPlayer)
{
    if(!otherControlPlayer)
    {
        return false;
    }
    else
    {
        this->controlPlayer = otherControlPlayer;
    }

    return true;
}
