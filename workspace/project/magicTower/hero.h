#ifndef __HERO__
#define __HERO__


#include "character.h"

class Hero : public Character
{
    private:
        //attribute
        int gold;

        //move
    public:
    
        const char * UseItem(const Item & item);                //使用物品，默认是不保存物品
        void ShowInfo() const;                                  //显示当前属性信息

};

#endif