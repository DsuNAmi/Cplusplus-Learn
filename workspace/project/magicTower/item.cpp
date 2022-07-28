#include "item.h"

Item::Item()
:effect(NOTHING),
 value(0),
 name(nullptr)
{
    this->name = new char[1];
}


Item::Item(Effect effect, 
           int value, 
           const char * name)
           : effect(effect),value(value),name(nullptr)
{
    int size = strlen(name);
    this->name = new char[size + 1];
    strcpy(this->name,name);
}

Item::Item(const Item & i)
:effect(i.effect),
 value(i.value),
 name(nullptr)
{
    int size = strlen(i.name);
    this->name = new char[size + 1];
    strcpy(this->name,i.name);
}

Item & Item::operator=(const Item & i)
{
    if(this != &i)
    {
        this->effect = i.effect;
        this->value = i.value;

        delete this->name;
        int size = strlen(i.name);
        this->name = new char[size + 1];
        strcpy(this->name,i.name);
    }
    return *this;
}

const char * Item::getName() const
{
    if(!this->name)
    {
        return "";
    }
    
    return this->name;
}

Item::~Item()
{
    delete [] this->name;
}