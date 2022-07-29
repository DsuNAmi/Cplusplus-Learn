#include "hero.h"

const char * Hero::UseItem(const Item & item)
{
    //可能会有多种增益的
    switch (item.getEffect())
    {
    case NOTHING:
        return NOTHINGEFFECT;
    case HEALING:
        this->health += item.getValue();
        return HEALINGEFFECT;
    case REMAGIC:
        this->magic += item.getValue();
        return REMAGICEFFECT;
    case ATTACKS:
        this->attack += item.getValue();
        return ATTACKSEFFECT;
    case DEFENSE:
        this->defense += item.getValue();
        return DEFENSEEFFECT;
    case ADDGOLD:
        this->gold += item.getValue();
        return ADDGOLDEFFECT;
    case DEHEALING:
        this->health -= item.getValue();
        this->health = this->health < 0 ? 0 : this->health;
        return DHEALINGEFFECT;
    case DEREMAGIC:
        this->magic -= item.getValue();
        this->magic = this->magic < 0 ? 0 : this->magic;
        return DREMAGICEFFECT;
    case DEATTACKS:
        this->attack -= item.getValue();
        this->attack = this->attack < 0 ? 0 : this->attack;
        return DATTACKSEFFECT;
    case DEDEFENSE:
        this->defense -= item.getValue();
        this->defense = this->defense < 0 ? 0 : this->defense;
        return DDEFENSEEFFECT;
    case DEADDGOLD:
        this->gold -= item.getValue();
        this->gold = this->gold < 0 ? 0 : this->gold;
        return DADDGOLDEFFECT;
    default:
        break;
    }
    return "";
}

void Hero::ShowInfo() const
{
    std::cout << "Nothing For Now!" << std::endl;
}