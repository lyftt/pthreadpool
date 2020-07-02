#ifndef _LOCK_H_
#define _LOCK_H_

#include <exception>
#include <pthread.h>
#include <semaphore.h>
#include <iostream>


//semaphore class
class sem
{
private:
    sem_t my_sem;    

public:
    sem()
    {
       if(sem_init(&my_sem,0,0) != 0)
       {
           std::cout<<"sem init error"<<std::endl;
           throw std::exception();
       }
    }

    ~sem()
    {
        sem_destroy(&my_sem);
    }

    bool wait()
    {
        return sem_wait(&my_sem) == 0;
    }

    bool post()
    {
        return sem_post(&my_sem) == 0;
    }
    
};


class locker
{
private:
    pthread_mutex_t my_mutex;

public:
    locker()
    {
        if(pthread_mutex_init(&my_mutex,NULL) != 0)
        {
            std::cout<<"mutex init error"<<std::endl;
            throw std::exception();
        }       
    }

    ~locker()
    {
        pthread_mutex_destroy(&my_mutex);
    }

    bool lock()
    {
        return pthread_mutex_lock(&my_mutex) == 0;
    }

    bool unlock()
    {
        return pthread_mutex_unlock(&my_mutex) == 0;
    }
};


class cond
{
private:
    pthread_mutex_t my_mutex;
    pthread_cond_t my_cond;

public:
    cond()
    {
        if(pthread_mutex_init(&my_mutex,NULL) != 0)
        {
            std::cout<<"pthread init error"<<std::endl;
            throw std::exception();
        }

        if(pthread_cond_init(&my_cond,NULL) != 0)
        {
            std::cout<<"pthread cond init error"<<std::endl;
            throw std::exception();
        }
    }

    ~cond()
    {
        pthread_mutex_destroy(&my_mutex);
        pthread_cond_destroy(&my_cond);
    }

    bool wait()
    {
        int ret = 0;
        pthread_mutex_lock(&my_mutex);
        ret = pthread_cond_wait(&my_cond,&my_mutex);
        pthread_mutex_unlock(&my_mutex);
        return ret == 0;
    }

    bool signal()
    {
        return pthread_cond_signal(&my_cond) == 0;
    }
};

#endif
