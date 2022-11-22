#ifndef __NGX_MEMORY_H__
#define __NGX_MEMORY_H__


#include <stddef.h>

class CMemory
{
    private:
        CMemory() {}
        static CMemory * mInstance;
    
    public:
        ~CMemory() {};
        void * AllocMemory(int memCount, bool ifmemset);
        void FreeMemory(void * point);


        static CMemory * GetInstance()
        {
            if(mInstance == nullptr)
            {
                if(mInstance == nullptr)
                {
                    mInstance = new CMemory();
                    static CGarhuishou cl;
                }
            }

            return mInstance;
        }


        class CGarhuishou
        {
            public:                
            ~CGarhuishou()
            {
                if(CMemory::mInstance)
                {
                    delete CMemory::mInstance;
                    CMemory::mInstance = nullptr;
                }
            }
        };
};

#endif