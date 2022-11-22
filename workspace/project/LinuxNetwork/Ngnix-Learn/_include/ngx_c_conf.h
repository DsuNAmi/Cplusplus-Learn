#ifndef __NGX_CONF_H__
#define __NGX_CONF_H__

#include <vector>

#include "ngx_global.h"

class CConfig
{
    private:
        static CConfig * m_instance;
        CConfig();
    
    public:
        std::vector<LPCConfItem> m_configItemList;  //这放的指针
        
    public:
        static CConfig * GetInstance()
        {
            if(m_instance == nullptr)
            {
                //双重检验，这里是多线程的时候，其实是需要加锁的
                if(m_instance == nullptr)
                {
                    m_instance = new CConfig();
                    static CGarhuishou cl;
                }
                //放锁
            }
            return m_instance;
        }

        class CGarhuishou
        {
            public: 
                ~CGarhuishou()
                {
                    if(CConfig::m_instance)
                    {
                        delete CConfig::m_instance;
                        CConfig::m_instance = nullptr;
                    }
                }
        };

        bool Load(const char * pconfName);  //load the config file
        const char * GetString(const char * pItemName);
        int GetIntDefault(const char * pItemName, const int def);

        ~CConfig();

};  


#endif