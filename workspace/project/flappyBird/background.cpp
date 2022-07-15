#include "background.h"

#include <iostream>


BackGround::BackGround() : uppperBound(0), bottomBound(MAXBOUNDROW),llefttBound(0),righttBound(MAXBOUNDCOL),_backGround(std::vector<std::vector<char>>(bottomBound,std::vector<char>(righttBound,' '))) {
    for(int UI = this->uppperBound; UI < this->bottomBound; ++UI)
    {
        this->_backGround[UI][0] = L;
        this->_backGround[UI][this->righttBound - 1] = R;
    }
    for(int UI = this->llefttBound; UI < this->righttBound; ++UI)
    {
        this->_backGround[0][UI] = U;
        this->_backGround[bottomBound - 1][UI] = B;
    }
    this->pillars = new int[INITPILLARS];
    this->ProducePillars();
}

void BackGround::GenerUI()
{
    for(auto v : this->_backGround)
    {
        for(auto x : v)
        {
            std::cout << x;
        }
        std::cout << std::endl;
    }
}

/**
 * @brief 生成一个柱子
 * 
 * @param pillar 柱子编号
 */
void BackGround::GenerPillar(int pillar)
{
    for(int UI = uppperBound + 1; UI < bottomBound - 1; ++UI)
    {
        this->_backGround[UI][this->pillars[pillar]] = P;
    }
    int startUI = ENDINTERVAL + rand() % STARTINTERVAL;
    int pInterval = MAXPILLARINTERVAL + rand() %  MINPILLARINTERVAL;
    for(int UI = startUI; UI <= startUI + pInterval; ++UI)
    {
        this->_backGround[UI][this->pillars[pillar]] = ' ';
    }
}

/**
 * @brief 生成柱子，第一个根柱子在第一次不生成
 * 
 */
void BackGround::ProducePillars()
{
    // srand(0);
    int interval = this->righttBound / INITPILLARS;
    this->pillars[0] = interval / 3;
    for(int pillar = 1; pillar < INITPILLARS; ++pillar) //开始时第一个柱子默认在第interval / 3位置，且不生成
    {
        this->pillars[pillar] = (interval * pillar) + (rand() % interval);
        this->GenerPillar(pillar);
    }
}



void BackGround::MovePillars()
{
    // srand(1);
    //先平移柱子
    for(int pillar = 0; pillar < INITPILLARS; ++pillar)
    {
        //如果平移时发现已经到平移尽头，重新等待生成新的柱子
        if(1 == this->pillars[pillar]) 
        {
            /* 随机时间等待新的柱子 */
            //清除柱子
            for(int UI = uppperBound + 1; UI < bottomBound - 1; ++UI)
            {
                this->_backGround[UI][this->pillars[pillar]] = ' ';
            }
            //生成一个倒计时
            int downTime = 1 + rand() % DOWNTIME;
            this->pillars[pillar] = -downTime; //倒计时
        }
        else if(!this->pillars[pillar])
        {
            //倒计时结束，生成新的柱子，从最后的位置
            this->pillars[pillar] = righttBound - 2;
            this->GenerPillar(pillar);
        }
        else if(0 > this->pillars[pillar])
        {
            ++(this->pillars[pillar]);//完成倒计时
        }
        else
        {
            /* 在新的位置生成柱子 */
            for(int UI = uppperBound + 1; UI < bottomBound - 1; ++UI)
            {
                this->_backGround[UI][this->pillars[pillar] - 1] = this->_backGround[UI][this->pillars[pillar]];
                //然后清除原来的柱子
                this->_backGround[UI][this->pillars[pillar]] = ' ';
            }
            //更新柱子的位置
            --(this->pillars[pillar]);
        }
    }
}

void BackGround::GenerBird(int x, int y, char bird)
{
    if(this->_backGround[y][x] == ' ') this->_backGround[y][x] = bird;
    else this->_backGround[y][x] += bird;
    // this->_backGround[y][x] = bird;
}

void BackGround::ShuffBird(int x, int y)
{
    this->_backGround[y][x] = ' ';
}
