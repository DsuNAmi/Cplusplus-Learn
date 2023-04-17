#ifndef __PLAYER__
#define __PLAYER__

#include <string>
#include <vector>


#include "setting.h"
#include "position.h"
#include "clubs.h"



class Player
{
    private:
        //basic information
        std::string             pName;
        int                     pAge;
        Clubs                   pClubs;
        DominatFoot             pDominaFoot;
        double                  pTransferValue;      //plyaer's transfer value

        //basic attribute
        //all attribute is limited in 0 ~ 20
            
        
        
        //field information
        std::vector<std::pair<Position,Proficiency>> pFieldPosition;



};

#endif