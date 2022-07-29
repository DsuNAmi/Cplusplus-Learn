#ifndef __TOWER__
#define __TOWER__


#include "layer.h"


typedef enum FloorDirection
{
    UP = 0,
    DOWN,
    RESET
} FloorDirection;

class Tower
{
    private:
        int layerSize;
        int curFloor;
        std::vector<Layer> layers;
        bool UpFloor();
        bool DownFloor();
        bool ResetFloor() {return curFloor = 0;}
    public:
        Tower();
        Tower(const std::vector<Layer> & layers);

        bool MoveLayer(FloorDirection direction);

        //额外功能，跳跃层数
        // bool JumpFloor(int targetFloor);

        int GetFloor() const {return this->curFloor + 1;}
        int GetIndexFloor() const {return this->curFloor;}
        void ShowCurFloor() const;

        void AddSomethingOnLayer(MapItems mis, int x, int y, int layer) {this->layers[layer].AddMapItems(mis,x,y);}
        void ReMoveSomethingOnLayer(int x, int y, int layer) {this->layers[layer].RemoveMapItems(x,y);}
};

#endif