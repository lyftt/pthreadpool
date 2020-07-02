#include "pthreadPool.h"
#include <iostream>


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
    std::cout<<"int run now"<<std::endl;
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
    std::cout<<"in worker now..."<<std::endl;

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

