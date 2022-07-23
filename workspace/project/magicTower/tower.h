#ifndef __TOWER__
#define __TOWER__


const int UPPERBOUND = 0;
const int BOTOMBOUND = 50;
const int LEFTTBOUND = 0;
const int RIGHTBOUND = 50;

const char L = '{';
const char R = '}';
const char U = '-';
const char B = '-';

#include <vector>

class Tower
{
    private:
        int ub;
        int bb;
        int lb;
        int rb;
        std::vector<std::vector<char>> layerMap;
    public:
        Tower();



};


#endif