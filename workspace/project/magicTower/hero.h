#ifndef __HERO__
#define __HERO__


#include "character.h"
#include "tower.h"

class Hero : public Character
{
    private:
        int gold;
        int xCoord;
        int yCoord;
        bool alive;
    public:
        Hero() : Character(), gold(0),xCoord(-1),yCoord(-1),alive(true) {};
        Hero(const char * name,
             int health, int magic,
             int attack, int defense,
             int gold,
             int xCoord, int yCoord);
        ~Hero() {}
        bool Check();

        bool GetAlive() const {return this->alive;}
        void ChangeAlive();


        const char * UseItem(const Item & item);                //使用物品
};

inline bool GamePass()
{

}

inline bool GameOver(const Hero & h)
{
    return !h.GetAlive();
}


#endif