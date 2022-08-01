#ifndef __CHARACTER__
#define __CHARACTER__


#include "item.h"

class Character
{
    protected:
        int health;    //血条
        int magic;     //蓝量
        char * name;            //名称
        int attack;
        int defense;
        int xCoord;
        int yCoord;
        bool alive;
        std::vector<Item> items;
        
        
        Character();
        Character(const char * name,
                  int health, int magic, 
                  int attack, int defense,
                  int xCoord, int yCoord,
                  bool alive,
                  const std::vector<Item> & items);
        Character(const Character & c);
        Character & operator=(const Character & c);
        virtual ~Character();
        void Check();
        bool SetCharacterPosition(int x, int y);

    public:
        bool Attack(Character * c);                                     //攻击方法
        virtual const char * UseItem(const Item & item);              //使用物品

        void ChangeAlive();

        //SeeMember;
        int getHealth() const {return this->health;}
        int getMagic() const {return this->magic;}
        int getAttack() const {return this->attack;}
        int getDefense() const {return this->defense;}
        const char * getName() const;
        bool GetAlive() {return this->alive;}
        virtual void ShowInfo() const {printf("No One Character!");}

};



#endif