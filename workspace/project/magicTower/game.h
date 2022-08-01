#ifndef __GAME__
#define __GAME__


#include <iostream>
#include <vector>
#include <cstring>

class Hero;
class Monster;

class Game
{
    public:
    
        static const int UPPERBOUND = 0;
        static const int BOTOMBOUND = 20;
        static const int LEFTTBOUND = 0;
        static const int RIGHTBOUND = 60;

        static const int rate = 2;   //放大倍率

        static const int layerSize = 5; //层数

        static const char L = '{';
        static const char R = '}';
        static const char U = '-';
        static const char B = '-';


        //mapFlag
        static const char HERO = 'H';
        static const char MONSTER = 'M';
        static const char VERTICALLWALL = '|';
        static const char HORIZONTALWALL = '-';
        static const char DOOR = '#';
        static const char UPSTAIRS = 'U';
        static const char DOWNSTAIRS = 'D';

        

        static inline bool GameOver(Hero * h) {return h;}

        static inline bool GamePass(Monster * m) {return m;}


        //判断是否越界
        static inline bool OutSide(int x, int y){
            return x <= Game::LEFTTBOUND     || 
               x >= Game::RIGHTBOUND - 1 || 
               y <= Game::UPPERBOUND     ||
               y >= Game::BOTOMBOUND - 1;
               
        }

        static inline const char * OutSideWarrning()
        {
            return "Out Side Bound!";
        }
};



#endif