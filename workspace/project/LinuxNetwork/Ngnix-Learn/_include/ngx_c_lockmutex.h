#ifndef __NGX_LOCKMUTEX_H__
#define __NGX_LOCKMUTEX_H__

#include <pthread.h>


//本类用于自动释放互斥量，防止忘记pthread_mutex_unlock的情况发生
class CLock
{
    public:
        CLock(pthread_mutex_t * pMutex)
        {
            mPMutex = pMutex;
            pthread_mutex_lock(pMutex);
        }

        ~CLock()
        {
            pthread_mutex_unlock(mPMutex); //解锁互斥量
        }

    private:
        pthread_mutex_t *mPMutex;
};

#endif