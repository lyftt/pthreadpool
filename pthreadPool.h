#ifndef _PTHREAD_POOL_H_
#define _PTHREAD_POOL_H_

#include "Singleton.h"
#include "locker.h"
#include <pthread.h>
#include <iostream>
#include <list>

template <typename T>
class pthreadPool: public Singleton<pthreadPool<T> >
{
    friend class Singleton<pthreadPool>;   //friend,because Singleton need to call private constructor

private:
    //constructor
    pthreadPool(int maxPthreadNum,int maxRequest);
    ~pthreadPool();
    void run();

private:
    static void* worker(void *args);     //worker for thread

public:
    bool appendRequest(T* request);

private:
    int maxPthreadNum = 0;   //max number of threads in pool
    int maxRequest = 0;
    pthread_t* pthreadArray;  //array to store pthread_t
    bool stopPoll = false;    //if to stop all pthreads in pool

    std::list<T*> workQueue;   //work queue
    locker myLocker;           //mutex lock for protect queue
    sem mySem;                 //number of request to process
};


template< typename T >
bool pthreadPool<T>::appendRequest(T * request)
{
    //need to lock
    myLocker.lock();
    if(maxRequest < workQueue.size())
    {
        //need to unlock
        myLocker.unlock();
        std::cout<<"append error: size over"<<std::endl;
        return false;
    }

    workQueue.push_back(request);
    mySem.post();       //add sem
    myLocker.unlock();  //unlock

    return true;
}

//deconstructor
template<typename T>
pthreadPool<T>::~pthreadPool()
{
    delete  []pthreadArray;
    stopPoll = true;    //stop all pthreads in pool
}


//run for thread, can access member variable
template <typename T>
void pthreadPool<T>::run()
{

    //std::cout<<"pthread"<<pthread_self()<<"are processing ";
    while(!stopPoll)
    {
        mySem.wait();      //wether have request to process
        myLocker.lock();   //lock
        
        if(workQueue.empty())
        {
            myLocker.unlock();
            continue;
        }

        myLocker.unlock();
        
        T* request = workQueue.front();   //take request from work queue
        workQueue.pop_front();            //pop the request
        
        if(!request)
        {
            continue;
        }
        request->process();       //process request
    }
}


//static method,can't access member variable
template<typename T>
void* pthreadPool<T>::worker(void *args)
{
    pthreadPool<T>* pool = (pthreadPool<T>*)args;
    //std::cout<<"in worker now..."<<std::endl;

    pool->run();

    return NULL;
}


//constructor
template<typename T>
pthreadPool<T>::pthreadPool(int maxPthreadNum,int maxRequest):maxPthreadNum(maxPthreadNum),maxRequest(maxRequest),pthreadArray(NULL),stopPoll(false)
{
        //argument error
    if(maxPthreadNum <= 0 || maxRequest < 0)
    {
        std::cout<<"maxPthreadNum error or maxRequest error"<<std::endl;
        throw std::exception();
    }

    pthreadArray = new pthread_t[maxPthreadNum];   //memory leak maybe
    if(!pthreadArray)
    {
        std::cout<<"pthreadArray new error"<<std::endl;
        throw std::exception();
    }

    //create pthread and detach self
    for(int i = 0; i < maxPthreadNum; ++i)
    {
        std::cout<<"start create pthreads in pool..."<<std::endl;

        if((pthread_create(pthreadArray+i,NULL,worker,this)) != 0)
        {
            std::cout<<"pthread_create error"<<std::endl;
            delete  []pthreadArray;   //delete
            throw std::exception();
        }

        if(pthread_detach(pthreadArray[i]))
        {
            std::cout<<"pthread_detach error"<<std::endl;
            delete  []pthreadArray;
            throw std::exception();
        }
    }
}

#endif

