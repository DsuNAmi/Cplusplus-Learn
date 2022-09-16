#include "player.h"

Player::Player():
playerName(""),national(NATIONAL::None),age(static_cast<UTT>(AgeStart)),height(150),dominantFoot(DOMINANT::None),
shoot(this->RandomAbility()),pass(this->RandomAbility()),control(this->RandomAbility()),defense(this->RandomAbility()),
involvement(this->RandomAbility()),leadership(this->RandomAbility()),professionalism(this->RandomAbility()),talent(this->RandomAbility()),
speed(this->RandomAbility()),explosive(this->RandomAbility()),physical(this->RandomAbility()),balance(this->RandomAbility())
{

}

Player::Player(std::string playerName,NATIONAL national, UTT age, double height, DOMINANT dominantFoot,
               UTT shoot, UTT pass, UTT control, UTT defense,
               UTT involvement, UTT leadership, UTT professionalism, UTT talent,
               UTT speed, UTT explosive, UTT physical, UTT balance):
               playerName(playerName),national(national),age(this->SetAge(age)),height(height),dominantFoot(dominantFoot),
               shoot(this->SetAbility(shoot)),pass(this->SetAbility(pass)),control(this->SetAbility(control)),defense(this->SetAbility(defense)),
               involvement(this->SetAbility(involvement)),leadership(this->SetAbility(leadership)),professionalism(this->SetAbility(professionalism)),talent(this->SetAbility(talent)),
               speed(this->SetAbility(speed)),explosive(this->SetAbility(explosive)),physical(this->SetAbility(physical)),balance(this->SetAbility(balance))
{

}

Player::Player(const Player & otherPlayer):
playerName(otherPlayer.playerName),national(otherPlayer.national),age(this->SetAge(otherPlayer.age)),height(otherPlayer.height),dominantFoot(otherPlayer.dominantFoot),
shoot(this->SetAbility(otherPlayer.shoot)),pass(this->SetAbility(otherPlayer.pass)),control(this->SetAbility(otherPlayer.control)),defense(this->SetAbility(otherPlayer.defense)),
involvement(this->SetAbility(otherPlayer.involvement)),leadership(this->SetAbility(otherPlayer.leadership)),professionalism(this->SetAbility(otherPlayer.professionalism)),talent(this->SetAbility(otherPlayer.talent)),
speed(this->SetAbility(otherPlayer.speed)),explosive(this->SetAbility(otherPlayer.explosive)),physical(this->SetAbility(otherPlayer.physical)),balance(this->SetAbility(otherPlayer.balance))
{

}

Player & Player::operator=(const Player & otherPlayer)
{
    if(this != &otherPlayer)
    {
        this->playerName = otherPlayer.playerName;
        this->national = otherPlayer.national;
        this->age = otherPlayer.age;
        this->height = otherPlayer.height;
        this->dominantFoot = otherPlayer.dominantFoot;

        this->shoot = otherPlayer.shoot;
        this->pass = otherPlayer.pass;
        this->control = otherPlayer.control;
        this->defense = otherPlayer.defense;

        this->involvement = otherPlayer.involvement;
        this->leadership = otherPlayer.leadership;
        this->professionalism = otherPlayer.professionalism;
        this->talent = otherPlayer.talent;

        this->speed = otherPlayer.speed;
        this->explosive = otherPlayer.explosive;
        this->physical = otherPlayer.physical;
        this->balance = otherPlayer.balance;
    }


    return *this;
}


UTT Player::SetAge(UTT age)
{
    if(age > Player::AgeLimit && age < Player::AgeStart)
    {
        printf("Wrong Age!");
        return -1;      //invaid Age
    }

    return age;
}


UTT Player::SetAbility(UTT abilityNumber)
{
    if(abilityNumber > Player::AbilityLimit && abilityNumber < Player::AbilityStart)
    {
        printf("Wrong ability value!");
        return -1;
    }

    return abilityNumber;
}


