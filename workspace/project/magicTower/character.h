#ifndef __CHARACTER__
#define __CHARACTER__

#include <iostream>

class Character
{
    private:
        int health;    //血条
        int magic;     //蓝量
        char * name;            //名称
        int attack;
        int defense;
        //禁用复制？
        Character(const Character & c);
        Character & operator=(const Character & c);
    public:
        Character() : health(0),magic(0),attack(0),defense(),name(nullptr) {}
        Character(const char * name,
                  int health, int magic, 
                  int attack, int defense);
        ~Character() {delete [] name;}
        void Check();

        //SeeMember;
        int getHealth() const {return this->health;}
        int getMagic() const {return this->magic;}
        int getAttack() const {return this->attack;}
        int getDefense() const {return this->defense;}
        const char * getName() const {return name;}


        bool Attack(Character & c); //攻击方法

};



#endif