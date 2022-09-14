#include <iostream>
#include <map>

#include "character.h"
#include "hero.h"
#include "tower.h"
#include "monster.h"

constexpr int INPUTSIZE = 2;

void TestAttack();
void TestTowerAndLayers();
void TestCreateMonster();


std::map<std::vector<int>, Monster*> monsters;

int main()
{
    printf("Do the Magic Tower\n");
    TestCreateMonster();
    return 0;
}


void TestCreateMonster()
{
    //创建五个怪物，一个怪物一层
    std::vector<Layer> layers(Game::layerSize);
    Tower magicTower(layers);
    //设定背景，墙，门，等
    //创建怪物
    //创建怪物身上的宝物
    std::vector<Item> items;
    //为每个怪物设定初始位置
    Monster m1 = Monster("monster1",100,100,100,100,2,3,true,items,100);
    monsters[{2,3,0}] = &m1;
    Monster m2 = Monster("monster2",100,100,100,100,12,25,true,items,100);
    monsters[{12,25,1}] = &m2;
    Monster m3 = Monster("monster3",100,100,100,100,14,45,true,items,100);
    monsters[{14,45,2}] = &m3;
    Monster m4 = Monster("monster4",100,100,100,100,34,20,true,items,100);
    monsters[{34,20,3}] = &m4;
    Monster m5 = Monster("monster5",100,100,100,100,4,5,true,items,100);
    monsters[{4,5,4}] = &m5;
    //添加怪物
    magicTower.AddSomethingOnLayer(MapItems::MONSTER,2,3,0);
    magicTower.AddSomethingOnLayer(MapItems::MONSTER,12,25,1);
    magicTower.AddSomethingOnLayer(MapItems::MONSTER,14,45,2);
    magicTower.AddSomethingOnLayer(MapItems::MONSTER,16,20,3);
    magicTower.AddSomethingOnLayer(MapItems::MONSTER,4,5,4);
    //每个怪物只是逻辑添加

    char inputChar[INPUTSIZE];

    printf("Press Enter to Start!\n");

    std::cin.getline(inputChar,2);

    while(strcmp(inputChar,"q") && strcmp(inputChar,"Q"))
    {
        printf("curFloor:  %d floor.",magicTower.GetFloor());
        printf("\n");
        magicTower.ShowCurFloor();
        printf("Press Enter u/U to upstairs or d/D to downstairs\n");
        std::cin.getline(inputChar,2);
        std::cout << inputChar;
        if(!strcmp(inputChar,"u") || !strcmp(inputChar,"U"))
        {
            if(!magicTower.MoveLayer(UP))
            {
                printf("already top floor!\n");
            }
            // std::system("cls");
        }
        else if(!strcmp(inputChar,"d") || !strcmp(inputChar,"D"))
        {
            if(!magicTower.MoveLayer(DOWN))
            {
                printf("already bottom floor!\n");
            }
            // std::system("cls");
        }
        else if(!strcmp(inputChar,"k") || !strcmp(inputChar,"K"))
        {
            //Kill Monster in curFloor;
            magicTower.ReMoveSomethingOnLayer(12,25,1);
            // monsters[{12,25,1}] = nullptr;
            monsters.erase({12,25,1});
        }
        else if((!strcmp(inputChar,"s") || !strcmp(inputChar,"S")))
        {
            //显示当前怪物信息
            if(monsters[{12,25,1}])
            {
                monsters[{12,25,1}]->ShowInfo();
            }
        }
        else
        {
            printf("error Input!\n");
        }

    }

    printf("Game Over!\n");
}

void TestTowerAndLayers()
{
    // char inputChar[2];
    // std::vector<Layer> layers(Game::layerSize);
    // Tower magicTower(layers);

    // printf("Press Enter to Start!\n");

    // std::cin.getline(inputChar,2);

    // while(strcmp(inputChar,"q") && strcmp(inputChar,"Q"))
    // {
    //     printf("curFloor:  %d floor.",magicTower.GetFloor());
    //     printf("\n");
    //     magicTower.ShowCurFloor();
    //     printf("Press Enter u/U to upstairs or d/D to downstairs\n");
    //     std::cin.getline(inputChar,2);
    //     std::cout << inputChar;
    //     if(!strcmp(inputChar,"u") || !strcmp(inputChar,"U"))
    //     {
    //         if(!magicTower.MoveLayer(UP))
    //         {
    //             printf("already top floor!\n");
    //         }
    //         // std::system("cls");
    //     }
    //     if(!strcmp(inputChar,"d") || !strcmp(inputChar,"D"))
    //     {
    //         if(!magicTower.MoveLayer(DOWN))
    //         {
    //             printf("already bottom floor!\n");
    //         }
    //         // std::system("cls");
    //     }
    // }

    // printf("Game Over!\n");
}

void TestAttack()
{
    // Character hero1("hujingrui",50,100,20,30);
    // Character monster("someOne",70,20,33,19);

    // printf("Start Status:\n");
    // printf("%s-health: %d\n",hero1.getName(),hero1.getHealth());
    // printf("%s-health: %d\n",monster.getName(),monster.getHealth());
    // printf("After attack? \n");
    // hero1.Attack(&monster);
    // printf("%s-health: %d\n",hero1.getName(),hero1.getHealth());
    // printf("%s-health: %d\n",monster.getName(),monster.getHealth());

    // printf("Using Healing : ");
    // Item healDrug(HEALING,30,"healing drugs");
    // printf("%s used %s!\n",hero1.getName(),healDrug.getName());
    // printf("%s\n",hero1.UseItem(healDrug));
    // printf("%s-health: %d\n",hero1.getName(),hero1.getHealth());
}