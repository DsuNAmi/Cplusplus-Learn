#ifndef __ITEM__
#define __ITEM__

#include <string.h>

#include "effect.h"

class Item
{
    private:
        Effect effect;
        int value;      //每个效果的value
        char * name;    //物品名称
        Item(const Item & item);
        Item & operator=(const Item & item);
    public:
        Item() : effect(NOTHING),value(0),name(nullptr) {};
        Item(Effect effect, int value, const char * name);
        ~Item() {delete [] this->name;}

        const char * getName() const;
        const Effect getEffect() const {return this->effect;}
        int getValue() const {return this->value;} 
};

#endif