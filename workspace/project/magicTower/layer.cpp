#include "layer.h"


Layer::Layer() : layerMap(std::vector<std::vector<char>>(Game::BOTOMBOUND,std::vector<char>(Game::RIGHTBOUND,' ')))
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


Layer::Layer(const Layer & l)
: layerMap(std::vector<std::vector<char>>(Game::BOTOMBOUND,std::vector<char>(Game::RIGHTBOUND,' ')))
{
    this->layerMap = l.layerMap;
}

Layer & Layer::operator=(const Layer & l)
{
    if(this != &l)
    {
        this->layerMap.clear();
        this->layerMap = std::vector<std::vector<char>>(Game::BOTOMBOUND,std::vector<char>(Game::RIGHTBOUND,' '));
        this->layerMap = l.layerMap;
    }
    return *this;
}


void Layer::AddMapItems(MapItems mis, int x, int y)
{
    switch (mis)
    {
    case HWALL:
        this->layerMap[x][y] = Game::HORIZONTALWALL;
        break;
    case VWALL:
        this->layerMap[x][y] = Game::VERTICALLWALL;
        break;
    case MapItems::DOOR:
        this->layerMap[x][y] = Game::DOOR;
    case MONSTER:
        this->layerMap[x][y] = Game::MONSTER;
        break;
    case HERO:
        this->layerMap[x][y] = Game::HERO;
        break;
    default:
        break;
    }
}


void Layer::ShowMap() const
{
    for(auto & uiv : layerMap)
    {
        for(auto & item : uiv)
        {
            std::cout << item;
        }
        std::cout << std::endl;
    }
}
