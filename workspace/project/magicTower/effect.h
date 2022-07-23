#ifndef __EFFECT__
#define __EFFECT__


#define NOTHINGEFFECT   "nothing happend"
#define HEALINGEFFECT   "healing"
#define REMAGICEFFECT   "magic added"
#define ATTACKSEFFECT   "attacked improved"
#define DEFENSEEFFECT   "defense improved"
#define ADDGOLDEFFECT   "golds added"
#define DHEALINGEFFECT  "health down"
#define DREMAGICEFFECT  "magic reduced"
#define DATTACKSEFFECT  "attack decreased"
#define DDEFENSEEFFECT  "defense decreased"
#define DADDGOLDEFFECT  "golds reduced"

/* 
这里涉及到全局变量的跨文件使用问题，

可以使用#define
以及使用extern const char* const， 但是要多一个文件出来进行定义，这里只能声明
"file.h"
extern const char * const str;
"file.cpp" 
const char * const str = "something";

或者使用类的静态成员函数，而不是静态成员变量
class C
{
    static const char * varName ;  //error,这里主要是多重定义问题
    //类外 const char * C::varName = "something";
    
    static const char * VarFuntion() {return "something";} //ok
};
*/


typedef enum Effect
{
    //无效道具
    NOTHING = 0,
    //正面增益
    HEALING = 1,    //回血
    REMAGIC = 2,    //回蓝
    ATTACKS = 4,    //加攻
    DEFENSE = 8,    //加防御
    ADDGOLD = 16,   //加金币
    //负面效果
    DEHEALING = 32,
    DEREMAGIC = 64,
    DEATTACKS = 128,
    DEDEFENSE = 256,
    DEADDGOLD = 512,
} Effect;


#endif