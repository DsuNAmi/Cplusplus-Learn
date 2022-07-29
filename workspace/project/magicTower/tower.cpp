#include "tower.h"

Tower::Tower()
:layerSize(Game::layerSize),
 curFloor(0),
 layers(std::vector<Layer>(layerSize))
{

}

Tower::Tower(const std::vector<Layer> & layers)
:layerSize(Game::layerSize),
 curFloor(0),
 layers(std::vector<Layer>(layerSize))
{
    for(int curf = 0; curf < layerSize; ++curf)
    {
        this->layers[curf] = layers[curf];
    }
}


bool Tower::UpFloor()
{
    if(curFloor < layerSize - 1)
    {
        ++curFloor;
        return true;
    }
    return false;
}


bool Tower::DownFloor()
{
    if(curFloor > 0)
    {
        --curFloor;
        return true;
    }
    return false;
}


bool Tower::MoveLayer(FloorDirection direction)
{
    switch (direction)
    {
        case UP:
            return this->UpFloor();
            break;
        case DOWN:
            return this->DownFloor();
            break;
        case RESET:
            return this->ResetFloor();
            break;
        default:
            break;
    }
    return true;
}

// bool Tower::JumpFloor(int targetFloor)
// {
//     //暂时不写
// }


void Tower::ShowCurFloor() const
{
    this->layers[this->curFloor].ShowMap();
}