#ifndef __PLAYER__
#define __PLAYER__


#include <string>


#include "settings.h"


class Player
{
    private:
        //ability range
        static const UTT AbilityLimit = 20;
        static const UTT AbilityStart = 1;
        static const UTT AgeLimit = 50;
        static const UTT AgeStart = 15;
        
        //player basic information
        std::string playerName;
        NATIONAL national;
        UTT age;
        double height;      //cm
        DOMINANT dominantFoot;
        
        //player ability
        //technology
        UTT shoot;              //shoot
        UTT pass;               //pass the ball
        UTT control;            //control the ball
        UTT defense;            //defense enemy
        
        //mind
        UTT involvement;                //the work Involvement
        UTT leadership;                 //leadership
        UTT professionalism;            //Professionalism
        UTT talent;                     //talent

        //body
        UTT speed;                      //max Speed;
        UTT explosive;                  //the time get to max Speed
        UTT physical;                   //the body
        UTT balance;                    //the balance



    public:
        Player();
        Player(std::string playerName,NATIONAL national, UTT age, double height, DOMINANT dominantFoot,
               UTT shoot, UTT pass, UTT control, UTT defense,
               UTT involvement, UTT leadership, UTT professionalism, UTT talent,
               UTT speed, UTT explosive, UTT physical, UTT balance);
        Player(const Player & otherPlayer);
        Player & operator=(const Player & otherPlayer);

        UTT SetAge(UTT age);
        UTT SetAbility(UTT abilityNumber);

        UTT RandomAbility() {return static_cast<UTT>(rand() % Player::AbilityLimit)
                             + Player::AbilityStart;}

             
};


#endif