#include <stdio.h>
#include <string.h>

void Rtrim(char * string)
{
    size_t len = 0;
    if(string == nullptr)
    {
        return;
    }

    len = strlen(string);

    while(len > 0 && string[len - 1] == ' ')
    {
        //删除尾部空格字符
        // string[--len] = '\0';
        string[--len] = 0;
    }
    return;
}

//首部的空格
void Ltrim(char * string)
{
    size_t len = 0;
    len = strlen(string);
    char * pTemp = string;
    if( (*pTemp) != ' ' )       //第一个是空格就不用再做处理了
    {
        return;
    }

    //遍历到第一个不是空格的地方
    while((*pTemp) != '\0')
    {
        if((*pTemp) == ' ')
        {
            ++pTemp;  
        }
        else
        {
            break;
        }
    }

    if((*pTemp) == '\0') //all space
    {
        *string = '\0';
        return;
    }

    //delete prefix space
    char * pTemp2 = string;
    while((*pTemp2) != '\0')
    {
        (*pTemp2) = (*pTemp);
        ++pTemp;
        ++pTemp2;
    }
    (*pTemp2) = '\0';
    return;
}