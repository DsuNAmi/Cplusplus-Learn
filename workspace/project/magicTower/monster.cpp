#include "monster.h"


Monster::Monster()
:Character(),
 dropGold(0)
{

}


Monster::Monster(const char * name,
                int health, int magic,
                int attack, int defense,
                int xCoord, int yCoord,
                bool alive,
                const std::vector<Item> & items,
                int dropGold)
                : Character(name,
                            health,magic,
                            attack,defense,
                            xCoord,yCoord,
                            alive,
                            items),
                 dropGold(dropGold)
{

}

Monster::Monster(const Monster & m)
:Character(m),
 dropGold(m.dropGold)
{

}

Monster & Monster::operator=(const Monster & m)
{
    if(this != &m)
    {
        Character::operator=(m);
        this->dropGold = m.dropGold;
    }
    return *this;
}

void Monster::ShowInfo() const
{
    std::cout << "MonsterName: " << this->name << std::endl;
    std::cout << "Attack: " << this->attack << " Defense: " << this->defense << std::endl;
}


int Monster::DropGolds() const
{
    if(!this->alive)
    {
        return this->dropGold;
    }
    return 0;
}
