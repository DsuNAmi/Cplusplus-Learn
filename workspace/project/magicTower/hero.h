#ifndef __HERO__
#define __HERO__


#include "character.h"

class Hero : public Character
{
    private:
        //attribute
        int gold;
        
        //move

    public:
        Hero();
        Hero(const char * name,
             int health, int magic,
             int attack, int defense,
             int xCoord, int yCoord,
             bool alive,
             const std::vector<Item> & items,
             int gold);
        Hero(const Hero & h);
        Hero & operator=(const Hero & h);
        




        const char * UseItem(const Item & item);                //使用物品，默认是不保存物品
        void ShowInfo() const;                                  //显示当前属性信息
        void ShowItems() const;                                 //显示当前物品信息
        
        const char * UseItemByIndex(int itemIndex);             //自选使用物品

        //移动
        void Move(Game::HeroMOve moveDirection);

};

#endif