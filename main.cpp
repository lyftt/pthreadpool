#include <iostream>
#include "pthreadPool.h"
#include <unistd.h>

class Request
{
public:
    Request(std::string s):secret(s)
    {

    }
    void process();
private:
    std::string secret;
};

void Request::process()
{
    //std::cout<<"============="<<secret<<"============="<<std::endl;
    std::cout<<"pthread"<<pthread_self()<<"is processing->"<<secret<<std::endl;
    sleep(2);
}

int main()
{
    pthreadPool<Request> *pool = pthreadPool<Request>::newInstance(3,10);    //create pool
    
    Request r1("r1");
    Request r2("r2");
    Request r3("r3");
    Request r4("r4");
    Request r5("r5");

    pool->appendRequest(&r1);
    pool->appendRequest(&r2);
    pool->appendRequest(&r3);
    pool->appendRequest(&r4);
    pool->appendRequest(&r5);
    
    sleep(10);

    pthreadPool<Request>::removeInstance();
    return 0;
}

