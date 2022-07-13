#ifndef __BACKGROUND__
#define __BACKGROUND__

#include <vector>

const int MAXBOUNDROW = 20;
const int MAXBOUNDCOL = 200;
const char L = '{'; //左边界形状
const char R = '}'; //有边界形状
const char U = '-';
const char B = '-'; //上、下边界形状
const char P = '*'; //柱子形状
const int INITPILLARS = 6; //同时存在的柱子数
//柱子的间隔范围
const int MINPILLARINTERVAL = 4;
const int MAXPILLARINTERVAL = 3;
//柱子开始间隔的起点范围
const int STARTINTERVAL = 4;
const int ENDINTERVAL = 7;

//柱子从新生成的倒计时0~3秒
const int DOWNTIME = 3;



class BackGround
{
    private:
        int uppperBound;
        int bottomBound;
        int llefttBound;
        int righttBound;
        std::vector<std::vector<char>> _backGround;
        void ProducePillars(); //生成柱子
        int * pillars;
        void GenerPillar(int pillar);

    public:
        BackGround();
        int GetUBound() {return this->uppperBound;}
        int GetBBound() {return this->bottomBound;}
        int GetLBound() {return this->llefttBound;}
        int GetRBound() {return this->righttBound;}

        void GenerUI();//创建UI
        void MovePillars();//移动柱子同时生成新的柱子
        ~BackGround() {delete pillars;}
};

#endif