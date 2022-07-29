#ifndef __ITEM__
#define __ITEM__


#include <cstring>
#include "effect.h"


class Item
{
    private:
        Effect effect;
        int value;      //每个效果的value
        char * name;    //物品名称
    public:
        Item();
        Item(Effect effect, int value, const char * name);
        Item(const Item & i);
        Item & operator=(const Item & i);
        ~Item();

        const char * getName() const;
        const Effect getEffect() const {return this->effect;}
        int getValue() const {return this->value;} 
};

#endif