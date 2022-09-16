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
        static const int layerEnd = 0;

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



        //Input
        static inline decltype(auto) UpLayer(bool uppercase) {return uppercase ? "U" : "u";}
        static inline decltype(auto) DownLayer(bool uppercase) {return uppercase ? "I" : "i";}
        static inline decltype(auto) UpHero(bool uppercase) {return uppercase ? "W" : "w";}
        static inline decltype(auto) DownHero(bool uppercase) {return uppercase ? "S" : "s";}
        static inline decltype(auto) LeftHero(bool uppercase) {return uppercase ? "A" : "a";}
        static inline decltype(auto) RightHero(bool uppercase) {return uppercase ? "D" : "d";}
        static inline decltype(auto) QuitGame(bool uppercase) {return uppercase ? "Q" : "q";}
        static inline decltype(auto) EnterGame(bool uppercase) {return uppercase ? "J" : "j";}





        static inline std::vector<int> ChangeLayer(bool up, int curLayer) {
            //这里有一个每一个层的位置信息，要知道在哪个层的楼梯口
            //测试的时候默认是1，1.
            return {1,1,up ? (curLayer + 1 >= Game::layerSize ? curLayer : curLayer + 1) : (curLayer - 1 < Game::layerEnd ? curLayer : curLayer - 1)};
        }


        enum class Pos
        {
            xCoord,
            yCoord,
            layer
        };


        enum class HeroMove {
            UPUP = 1,
            DOWN,
            LEFT,
            RIGH,
        };

    
        static inline bool GameOver(Hero * h) {return h;}

        static inline bool GamePass(Monster * m) {return m;}


        //判断是否越界
        static inline bool OutSide(int x, int y){
            return y <= Game::LEFTTBOUND     || 
               y >= Game::RIGHTBOUND - 1 || 
               x <= Game::UPPERBOUND     ||
               x >= Game::BOTOMBOUND - 1;
               
        }

        static inline const char * OutSideWarrning()
        {
            return "Out Side Bound!";
        }
};



#endif