#include "character.h"


#include <string.h>

Character::Character(const char * name,
                     int health, int magic, 
                     int attack, int defense)
                     : name(nullptr), health(health), magic(magic), attack(attack), defense(defense)
{
    //check
    this->Check();

    //memset
    int nameSize = strlen(name);
    this->name = new char[nameSize + 1];
    strcpy(this->name,name);
}


void Character::Check()
{
    this->health = this->health < 0 ? 0 : this->health;
    this->magic = this->magic < 0 ? 0 : this->magic;
    this->attack = this->attack < 0 ? 0 : this->attack;
    this->defense = this->defense < 0 ? 0 : this->defense;
}


bool Character::Attack(Character & c)
{
    if(this == &c) return false; //do not attack
    int attackValue = this->attack < c.defense ? 0 : this->attack - c.defense;
    int damageValue = this->defense > c.attack ? 0 : c.attack - this->defense;
    while(this->health > 0 && c.health > 0)
    {
        c.health -= attackValue;
        if(c.health <= 0)
        {
            break;
        }
        this->health -= damageValue;
    }
    this->health = this->health < 0 ? 0 : this->health;
    c.health = c.health < 0 ? 0 : c.health;

    return true;
}