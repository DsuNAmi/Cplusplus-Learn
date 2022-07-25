#ifndef __LAYER__
#define __LAYER__

#include <vector>

#include "game.h"


class layer
{
    private:
        int ub;
        int bb;
        int lb;
        int rb;
        std::vector<std::vector<char>> layerMap;
    public:
        layer();
};


#endif