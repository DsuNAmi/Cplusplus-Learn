#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

#include "ngx_func.h"
#include "ngx_c_conf.h"

CConfig * CConfig::m_instance = nullptr;

CConfig::CConfig()
{

}


CConfig::~CConfig()
{
    // std::vector<LPCConfItem>::iterator pos;
    // for(pos = m_configItemList.begin(); pos != m_configItemList.end(); ++pos)
    // {
    //     delete (*pos);      //还能这么释放内存？
    // }
    // m_configItemList.clear();
}

bool CConfig::Load(const char * pconfName)
{
    FILE * fp;
    fp = fopen(pconfName,"r");
    if(fp == nullptr)
    {
        return false;
    }

    //读取每一行配置文件，但是限定每一行的长度
    char lineBuf[501];

    while(!feof(fp))        //这里是当文件不走到最后的时候
    {
        if(fgets(lineBuf,500,fp) == nullptr)
        {
            continue;
        }


        if(lineBuf[0] == 0) //这里表示是空行
        // if(lineBuf[0] == '\0') ?
        {
            continue;
        }

        //处理注释
        if(*lineBuf == ';'  ||
           *lineBuf == ' '  ||
           *lineBuf == '#'  ||
           *lineBuf == '\t' ||
           *lineBuf == '\n')
        {
            continue;
        }

    
    lblprocstring:
        //后面有换行，回车，空格都截取
        if(strlen(lineBuf) > 0)
        {
            if(lineBuf[strlen(lineBuf) - 1] == 10 ||
               lineBuf[strlen(lineBuf) - 1] == 13 ||
               lineBuf[strlen(lineBuf) - 1] == 32)
            {
                lineBuf[strlen(lineBuf) - 1] = 0;
                goto lblprocstring;
            }
        }

        if(0 == lineBuf[0])
        {
            continue;
        }
        //这里是防止出现非有效信息的时候，就是删除完了后面的内容，结果出现空行
        if(*lineBuf == '[')     //跳过[开头的内容，直接continue
        {
            continue;
        }

        char * pTemp = strchr(lineBuf, '=');

        if(pTemp != nullptr)
        {
            LPCConfItem pConfItem = new CConfItem;
            memset(pConfItem, 0, sizeof(CConfItem));
            //等号两边的分别复制过去
            strncpy(pConfItem->itemName,lineBuf,(int)(pTemp - lineBuf));
            //右边不涉及到长度了
            strcpy(pConfItem->itemContent, pTemp + 1);

            Rtrim(pConfItem->itemName);
            Ltrim(pConfItem->itemName);
            Rtrim(pConfItem->itemContent);
            Ltrim(pConfItem->itemContent);

            // printf("itemname=%s | itemcontent=%s\n",pConfItem->itemName,pConfItem->itemContent);
            this->m_configItemList.emplace_back(pConfItem);     //内存在结束的时候要放掉

        }
    }

    fclose(fp);     //很重要
    return true;
}

const char * CConfig::GetString(const char * pItemName)
{
    std::vector<LPCConfItem>::iterator pos;     //这个不是在循环里面定义的
    for(pos = m_configItemList.begin(); pos != m_configItemList.end(); ++pos)
    {
        //忽略大小写比较字符串
        if(strcasecmp((*pos)->itemName,pItemName) == 0)
        {
            return (*pos)->itemContent;
        }
    }
    return nullptr;
}

int CConfig::GetIntDefault(const char * pItemName, const int def)
{
    std::vector<LPCConfItem>::iterator pos;
    for(pos = m_configItemList.begin(); pos != m_configItemList.end(); ++pos)
    {
        if(strcasecmp((*pos)->itemName,pItemName) == 0)
        {
            return atoi((*pos)->itemContent);
        }
    }
    return def;
}






