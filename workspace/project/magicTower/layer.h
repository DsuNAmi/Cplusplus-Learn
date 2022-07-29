#ifndef __LAYER__
#define __LAYER__


#include "game.h"


typedef enum MapItems
{
    HWALL = 0,
    VWALL,
    DOOR,
    MONSTER,
    HERO
} MapItems;

class Layer
{
    private:
        std::vector<std::vector<char>> layerMap;
    public:
        Layer();
        //可以通过设定的对象来创建完整的内容
        // Layer(params);
        Layer(const Layer & l);
        Layer & operator=(const Layer & l);

        void AddMapItems(MapItems mis, int x, int y);
        void RemoveMapItems(int x, int y) {this->layerMap[x][y] = ' ';}
        

        void ShowMap() const;
};


#endif