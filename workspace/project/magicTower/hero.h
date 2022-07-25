#ifndef __HERO__
#define __HERO__


#include "character.h"

class Hero : public Character
{
    private:
        int gold;
    public:
    
        const char * UseItem(const Item & item);                //使用物品，默认是不保存物品

};

#endif