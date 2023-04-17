// this program just do some chose for me

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <cctype>
#include <ctime>
#include <unordered_map>


// what I want to do just more randomly


//so, I will set some rules

//the way what I think is to down the probability with the game(or other someting) price

//I mean... the price is higher and the probability is lower

//at first, I need some candicates

//2023/01/20
//what confuse me is to buy some game, but they cost so much?

//so, at that day, 1 HK is 0.8659 RMB, 1JPY is 0.0521 RMB



//The Last of Us part I(PS5), price: 568HK
//Grand Theft Auto Five(PS5), price: 159HK



//fixed cost: 10500JPY

//Fire Emblem : Engage, price: 7600JPY, Not in Pool, 
//Mario Rabbits, price : 6500JPY
//Super Mario Maker, price : 6578JPY
//Nintendo Special, price : 7920JPY
//Super Mario 3D World, price : 6578JPY
//Pokmen Purple, price : 6500JPY

const std::string JPY = "JPY";
const std::string HK = "HK";
constexpr double J2R = 0.0521;
constexpr double H2R = 0.8695;
constexpr int Times = 1000;

std::map<std::string, std::string> NameToPrice;
std::map<std::string, double> GamePrice;
std::map<std::string, double> GameProbability;
std::vector<std::string> GameNames;
std::map<std::string, int> answers;



void GerneratePools(int Times, int has, std::vector<std::string> & GamePool, std::string GameName)
{
    std::map<int, bool> isHave;
    int i = 0;
    // std::cout << has << std::endl;
    // std::cin >> i;
    while(i < has)
    {
        int index = rand() % Times;
        // std::cout << index << std::endl;
        if(isHave[index] || GamePool[index] != "None") continue;
        GamePool[index] = GameName;
        isHave[index] = true;
        ++i;
        // std::cout << i << std::endl;
    }
}

std::string FixAnser(std::map<std::string, std::string> & ans)
{
    std::string ansString = "[";
    ansString += GameNames[0];
    ansString += ans[GameNames[0]] == GameNames[0] ? ": Yes, " : ": No, ";
    ansString += GameNames[1];
    ansString += ans[GameNames[1]] == GameNames[1] ? ": Yes, " : ": No, ";
    ansString += ans["Nintendo"];
    ansString += "]";

    // return ansString;
    ++answers[ansString];
    return "";
}

void Choose()
{
    //choose the first;, buy or not
    //[The Last of Us part I: Yes/No, Grand Theft Auto Five: Yes/No, SomeNintendoGame]
    //sim 1000000 times
    std::map<std::string, std::string> ans;
    ans[GameNames[0]] = "None";
    ans[GameNames[1]] = "None";
    ans["Nintendo"] = "Surely";

    // for(auto [k ,v] : GameProbability)
    // {
    //     std::cout << v << std::endl;
    // }

    // int i ;
    // std::cin >> i;

    std::vector<std::string> GameIPool(Times, "None");
    GerneratePools(Times, Times * GameProbability[GameNames[0]], GameIPool, GameNames[0]);
    int indexI = rand() % Times;
    ans[GameNames[0]] = GameIPool[indexI];


    std::vector<std::string> GameIIPool(Times, "None");
    GerneratePools(Times, Times * GameProbability[GameNames[1]], GameIIPool, GameNames[1]);
    int indexII = rand() % Times;
    ans[GameNames[1]] = GameIIPool[indexII];

    std::vector<std::string> GameIIIPool(Times, "None");
    GerneratePools(Times, Times * GameProbability[GameNames[2]], GameIIIPool, GameNames[2]);
    GerneratePools(Times, Times * GameProbability[GameNames[3]], GameIIIPool, GameNames[3]);
    GerneratePools(Times, Times * GameProbability[GameNames[4]], GameIIIPool, GameNames[4]);
    GerneratePools(Times, Times * GameProbability[GameNames[5]], GameIIIPool, GameNames[5]);
    GerneratePools(Times, Times * GameProbability[GameNames[6]], GameIIIPool, GameNames[6]);
    int indexIII = rand() % Times;
    ans["Nintendo"] = GameIIIPool[indexIII];

    FixAnser(ans);
    // std::cout << FixAnser(ans) << std::endl;
}

void SetProbability()
{
    double sumPrice = 0.0;
    for(auto && [k, v] : GamePrice)
    {
        sumPrice += v;
    }
    for(auto && [k ,v] : GamePrice)
    {
        GameProbability[k] = (sumPrice - v) / sumPrice;
    }
    double forOne = 0.0;
    for(auto && [k ,v] : GameProbability)
    {
        forOne += v;
    }
    for(auto && [k, v] : GameProbability)
    {
        v = v / forOne;
    }

}

std::string GetCurrency(std::string price, double & Doubleprice)
{
    std::string currency;
    for(auto c : price)
    {
        if(!isdigit(c))
        {
            currency += c;
        }
        else
        {
            int cNumber = c - '0';
            Doubleprice = 10 * Doubleprice + c;
        }
    }

    return currency;
}

void ChangeToRMB()
{
    for(auto & [k, v] : NameToPrice)
    {
        double price = 0.0;
        std::string change = GetCurrency(v,price);
        if(change == JPY)
        {
            GamePrice[k] = price * J2R;
        }
        else if(change == HK)
        {
            GamePrice[k] = price * H2R;
        }
    }
}

void SetNameToPrice()
{
    GameNames.emplace_back("The Last of Us part I");
    GameNames.emplace_back("Grand Theft Auto Five");
    GameNames.emplace_back("Mario Rabbits");
    GameNames.emplace_back("Super Mario Maker");
    GameNames.emplace_back("Nintendo Special");
    GameNames.emplace_back("Super Mario 3D World");
    GameNames.emplace_back("Pokmen Purple");
    // GameNames.emplace_back("");

    NameToPrice["The Last of Us part I"] = "568HK";
    NameToPrice["Grand Theft Auto Five"] = "159HK";
    NameToPrice["Mario Rabbits"] = "6500JPY";
    NameToPrice["Super Mario Maker"] = "6578JPY";
    NameToPrice["Nintendo Special"] = "7920JPY";
    NameToPrice["Super Mario 3D World"] = "6578JPY";
    NameToPrice["Pokmen Purple"] = "6500JPY";

}

int main()
{
    srand(time(NULL));
    SetNameToPrice();
    ChangeToRMB();
    SetProbability();
    for(int i = 0; i < 100000; ++i)
    {
        Choose();
    }
    for(auto && [k ,v] : answers)
    {
        std::cout << k  << "---" << v << std::endl;
    }
    return 0;
}



//The Last of Us part I(PS5), price: 568HK
//Grand Theft Auto Five(PS5), price: 159HK



//fixed cost: 10500JPY

//Fire Emblem : Engage, price: 7600JPY, Not in Pool, 
//Mario Rabbits, price : 6500JPY
//Super Mario Maker, price : 6578JPY
//Nintendo Special, price : 7920JPY
//Super Mario 3D World, price : 6578JPY
//Pokmen Purple, price : 6500JPY