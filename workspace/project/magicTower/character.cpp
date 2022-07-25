#include "character.h"

Character::Character()
:name(nullptr),
 health(0),magic(0),
 attack(0),defense(0),
 xCoord(1),yCoord(1),
 alive(true)
{
    this->name = new char[1];
}

Character::Character(const char * name,
                     int health, int magic, 
                     int attack, int defense,
                     int xCoord, int yCoord,
                     bool alive,
                     const std::vector<Item> & items)
                     : name(nullptr), 
                       health(health), magic(magic), 
                       attack(attack), defense(defense),
                       xCoord(xCoord), yCoord(yCoord),
                       alive(true)
{
    //check
    this->Check();
    this->SetCharacterPosition(xCoord,yCoord);
    
    //memset
    int nameSize = strlen(name);
    this->name = new char[nameSize + 1];
    strcpy(this->name,name);
    
    //vector
    for(auto item : items)
    {
        this->items.emplace_back(item);
    }
}


Character::Character(const Character & c)
:name(nullptr),
 health(c.health),magic(c.magic),
 attack(c.attack),defense(c.defense),
 xCoord(c.xCoord),yCoord(yCoord),
 alive(c.alive)
{
    //check
    this->Check();
    this->SetCharacterPosition(xCoord,yCoord);

    //memset
    int nameSize = strlen(c.name);
    this->name = new char[nameSize + 1];
    strcpy(this->name,c.name);

    //vector
    for(auto item : c.items)
    {
        this->items.emplace_back(item);
    }
}

Character & Character::operator=(const Character & c)
{
    if(this != &c)
    {
        this->health = c.health;
        this->magic = c.magic;
        this->attack = c.attack;
        this->defense = c.defense;
        this->xCoord = c.xCoord;
        this->yCoord = c.yCoord;
        this->alive = c.alive;

        //check
        this->Check();
        this->SetCharacterPosition(xCoord,yCoord);

        //memset
        delete [] this->name;
        int nameSize = strlen(c.name);
        this->name = new char[nameSize + 1];
        strcpy(this->name,c.name);

        //vector
        this->items.clear();
        for(auto item : c.items)
        {
            this->items.emplace_back(item);
        }
    }
    return *this;
}

const char * Character::getName() const
{
    if(!this->name)
    {
        return "";
    }
    
    return this->name;
}

bool Character::SetCharacterPosition(int x, int y)
{
    if(x <= Game::LEFTTBOUND     || 
       x >= Game::RIGHTBOUND - 1 || 
       y <= Game::UPPERBOUND     ||
       y >= Game::BOTOMBOUND - 1)
    {
        this->xCoord = 1;
        this->yCoord = 1;
        return false;
    }
    this->xCoord = xCoord;
    this->yCoord = yCoord;
    return true;
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

void Character::ChangeAlive()
{
    if(this->health <= 0)
    {
        this->alive = false;
    }
}
