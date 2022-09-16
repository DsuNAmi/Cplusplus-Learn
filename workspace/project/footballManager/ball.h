#ifndef __BALL__
#define __BALL__

#include "player.h"


class Ball
{
    private:
        Player * controlPlayer;         //who controls the ball?
        bool hasPlayer;                 //anyone controls the ball
    
    public:
        Ball();
        Ball(Player * controlPlayer);
        ~Ball();

        bool ChangeControlPlayer(Player * otherControlPlayer);
        bool AnyoneControl() {return this->hasPlayer;}
};


#endif