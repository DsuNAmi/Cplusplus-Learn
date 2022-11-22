#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ngx_c_crc32.h"

CCRC32 * CCRC32::mInstance = nullptr;

//构造函数
CCRC32::CCRC32()
{
    InitCrc32Table();
}

//析构函数
CCRC32::~CCRC32()
{

}

 unsigned int CCRC32::Reflect(unsigned int ref, char ch)
 {
    unsigned int value(0);

    for(int i = 1; i < (ch + 1); ++i)
    {
        if( ref & 1)
        {
            value |= 1 << (ch - i);
        }
        ref >>= 1;
    }
    return value;
 }

 //初始化crc32表

void CCRC32::InitCrc32Table()
{
    unsigned int ulPolynomial = 0x04c11db7;

    for(int i = 0; i < 0xFF; ++i)
    {
        crc32Table[i] = Reflect(i, 8) << 24;

        for(int j = 0; j < 8; ++j)
        {
            crc32Table[i] = (crc32Table[i] << 1) ^ (crc32Table[i] & (i << 31) ? ulPolynomial : 0);
        }

        crc32Table[i] = Reflect(crc32Table[i],32);
    }
}

int CCRC32::GetCRC(unsigned char * buffer, unsigned int dwSize)
{
    unsigned int  crc(0xffffffff);
	int len;

    len = dwSize;
    while(len--)
    {
        crc = (crc >> 8) ^ crc32Table[(crc & 0xFF) ^ *buffer++];
    }
    return crc^0xffffffff;
}