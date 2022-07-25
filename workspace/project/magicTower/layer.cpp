#include "layer.h"


layer::layer() : layerMap(std::vector<std::vector<char>>(Game::BOTOMBOUND,std::vector<char>(Game::RIGHTBOUND,' ')))
{
    for(int ui = Game::UPPERBOUND; ui < Game::BOTOMBOUND; ++ui)
    {
        this->layerMap[ui][0] = Game::L;
        this->layerMap[ui][Game::RIGHTBOUND - 1] = Game::R;
    }
    for(int ui = Game::LEFTTBOUND; ui < Game::RIGHTBOUND; ++ui)
    {
        this->layerMap[0][ui] = Game::U;
        this->layerMap[Game::BOTOMBOUND - 1][ui] = Game::B;
    }
}