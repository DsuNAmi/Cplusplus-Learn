#include <iostream>

#include "character.h"
#include "item.h"

int main()
{
    printf("Do the Magic Tower");


    Character hero1("hujingrui",50,100,20,30);
    Character monster("someOne",70,20,33,19);

    printf("Start Status:\n");
    printf("%s-health: %d\n",hero1.getName(),hero1.getHealth());
    printf("%s-health: %d\n",monster.getName(),monster.getHealth());
    printf("After attack? \n");
    hero1.Attack(monster);
    printf("%s-health: %d\n",hero1.getName(),hero1.getHealth());
    printf("%s-health: %d\n",monster.getName(),monster.getHealth());


    return 0;
}