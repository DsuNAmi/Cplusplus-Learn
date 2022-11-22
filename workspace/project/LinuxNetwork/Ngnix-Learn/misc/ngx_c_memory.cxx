#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ngx_c_memory.h"


CMemory * CMemory::mInstance = nullptr;



void *CMemory::AllocMemory(int memCount, bool ifmemset)
{
    void* tempData = (void *)new char[memCount];
    if(ifmemset)
    {
        memset(tempData,0,memCount);
    }
    return tempData;
}


void CMemory::FreeMemory(void * point)
{
    delete [] ((char*)point);
}