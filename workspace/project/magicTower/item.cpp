#include "item.h"


Item::Item(Effect effect, 
           int value, 
           const char * name)
           : effect(effect),value(value),name(nullptr)
{
    int size = strlen(name);
    this->name = new char[size + 1];
    strcpy(this->name,name);
}

const char * Item::getName() const
{
    if(!this->name)
    {
        return "";
    }
    
    return this->name;
}