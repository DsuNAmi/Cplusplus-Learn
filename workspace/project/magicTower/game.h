#ifndef __GAME__
#define __GAME__



class Hero;
class Monster;

class Game
{
    public:
    
    static const int UPPERBOUND = 0;
    static const int BOTOMBOUND = 50;
    static const int LEFTTBOUND = 0;
    static const int RIGHTBOUND = 50;

    static const char L = '{';
    static const char R = '}';
    static const char U = '-';
    static const char B = '-';

    static inline bool GameOver(Hero * h) {return h;}

    static inline bool GamePass(Monster * m) {return m;}
};



#endif