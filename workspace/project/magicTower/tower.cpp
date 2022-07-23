#include "tower.h"


Tower::Tower() : layerMap(std::vector<std::vector<char>>(BOTOMBOUND,std::vector<char>(RIGHTBOUND,' ')))
{
    for(int ui = UPPERBOUND; ui < BOTOMBOUND; ++ui)
    {
        this->layerMap[ui][0] = L;
    }
}