#include "character.h"


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

const char * Character::getName() const
{
    if(!this->name)
    {
        return "";
    }
    
    return this->name;
}


void Character::Check()
{
    this->health = this->health < 0 ? 0 : this->health;
    this->magic = this->magic < 0 ? 0 : this->magic;
    this->attack = this->attack < 0 ? 0 : this->attack;
    this->defense = this->defense < 0 ? 0 : this->defense;
}


bool Character::Attack(Character * c)
{
    if(this == c) return false; //do not attack self
    int attackValue = this->attack < c->defense ? 0 : this->attack - c->defense;
    int damageValue = this->defense > c->attack ? 0 : c->attack - this->defense;
    while(this->health > 0 && c->health > 0)
    {
        c->health -= attackValue;
        if(c->health <= 0)
        {
            break;
        }
        this->health -= damageValue;
    }
    this->health = this->health < 0 ? 0 : this->health;
    c->health = c->health < 0 ? 0 : c->health;

    return true;
}

const char * Character::UseItem(const Item & item)
{
    return "";
    // //可能会有多种增益的
    // switch (item.getEffect())
    // {
    // case NOTHING:
    //     return NOTHINGEFFECT;
    // case HEALING:
    //     this->health += item.getValue();
    //     return HEALINGEFFECT;
    // case REMAGIC:
    //     this->magic += item.getValue();
    //     return REMAGICEFFECT;
    // case ATTACKS:
    //     this->attack += item.getValue();
    //     return ATTACKSEFFECT;
    // case DEFENSE:
    //     this->defense += item.getValue();
    //     return DEFENSEEFFECT;
    // case DEHEALING:
    //     this->health -= item.getValue();
    //     this->health = this->health < 0 ? 0 : this->health;
    //     return DHEALINGEFFECT;
    // case DEREMAGIC:
    //     this->magic -= item.getValue();
    //     this->magic = this->magic < 0 ? 0 : this->magic;
    //     return DREMAGICEFFECT;
    // case DEATTACKS:
    //     this->attack -= item.getValue();
    //     this->attack = this->attack < 0 ? 0 : this->attack;
    //     return DATTACKSEFFECT;
    // case DEDEFENSE:
    //     this->defense -= item.getValue();
    //     this->defense = this->defense < 0 ? 0 : this->defense;
    //     return DDEFENSEEFFECT;
    // default:
    //     break;
    // }
    // return "";
}
