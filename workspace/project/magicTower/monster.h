#ifndef __MONSTER__
#define __MONSTER__


#include "character.h"

class Monster : public Character
{
    private:
        int dropGold;       //死亡掉落的金币
        const char * UseItem(const Item & item) {return Character::UseItem(item);}
    public:
        Monster();
        Monster(const char * name,
                int health, int magic,
                int attack, int defense,
                int xCoord, int yCoord,
                bool alive,
                const std::vector<Item> & items,
                int dropGold);
        Monster(const Monster & m);
        Monster & operator=(const Monster & m);


        int DropGolds() const;

        void ShowInfo() const;
};


#endif