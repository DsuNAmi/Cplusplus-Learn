#ifndef __BALL__
#define __BALL__

#include "player.h"

#include <memory>


class Ball
{
    private:
        Player * controlPlayer;         //who controls the ball?
        // std::unique_ptr<Player> controlPlayer;
        bool hasPlayer;                 //anyone controls the ball
    
    public:
        Ball();
        Ball(Player * controlPlayer);
        ~Ball();

        bool ChangeControlPlayer(Player * otherControlPlayer);
        bool AnyoneControl() {return this->hasPlayer;}
};


#endif