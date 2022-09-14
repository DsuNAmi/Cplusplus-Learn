#include "hero.h"



Hero::Hero()
:Character(),
 gold(0)
{

}

Hero::Hero(const char * name,
           int health, int magic,
            int attack, int defense,
            int xCoord, int yCoord,
            bool alive,
            const std::vector<Item> & items,
            int gold)
            : Character(name,
                        health,magic,
                        attack,defense,
                        xCoord,yCoord,
                        alive,
                        items),
              gold(gold)
{

}


Hero::Hero(const Hero & h)
:Character(h),
 gold(h.gold)
{

}

Hero & Hero::operator=(const Hero & h)
{
    if(this != &h)
    {
        Character::operator=(h);
        this->gold = h.gold;
    }
    return *this;
}

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

const char* Hero::UseItemByIndex(int itemIndex)
{
    int maxIndex = static_cast<int>(this->items.size());
    if(itemIndex >= maxIndex || itemIndex < 0)
    {
        return "";
    } 
    return this->UseItem(this->items[itemIndex]);
}

void Hero::ShowInfo() const
{
    std::cout << "Nothing For Now! Attributes" << std::endl;
}

void Hero::ShowItems() const
{
    int length = static_cast<int>(this->items.size());
    std::cout << "Items:" << std::endl;
    for(int item = 0; item < length; ++item)
    {
        std::cout << item + 1 << ". " << this->items[item].getName() << std::endl;
    }
    std::cout << "---------------------------------------" << std::endl;
}


void Hero::Move(Game::HeroMOve moveDirection)
{
    
}
