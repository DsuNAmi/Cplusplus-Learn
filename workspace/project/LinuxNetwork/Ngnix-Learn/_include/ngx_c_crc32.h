#ifndef __NGX_C_CRC32_H__
#define __NGX_C_CRC32_H__

#include <stddef.h>


class CCRC32
{
    private:
        CCRC32();
        static CCRC32 * mInstance;
    
    public:
        ~CCRC32();

        static CCRC32 * GetInstance()
        {
            if(mInstance == nullptr)
            {
                if(mInstance == nullptr)
                {
                    mInstance = new CCRC32();
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
                    if(CCRC32::mInstance)
                    {
                        delete CCRC32::mInstance;
                        CCRC32::mInstance = nullptr;
                    }
                }
        };

        
        void InitCrc32Table();

        unsigned int Reflect(unsigned int ref, char ch);

        int GetCRC(unsigned char * buffer, unsigned int dwSize);

        unsigned int crc32Table[256]; //looup table arrays


};


#endif