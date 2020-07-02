#ifndef _SINGLETON_H_
#define _SINGLETON_H_

#define NULL 0

template <typename T>
class Singleton
{
public:
    static T* newInstance()
    {
        if(!instance)
        {
            instance = new T;
        }

        return instance;
    }

    static T* newInstance(int arg1,int arg2)
    {
        if(!instance)
        {
            instance = new T(arg1,arg2);
        }

        return instance;
    }

    static T* getInstance()
    {
        if(!instance)
        {
            instance = new T;
        }

        return instance;
    }

    static void removeInstance()
    {
        if(instance)
        {
            delete instance;
            instance = NULL;
        }
    }

protected:
    Singleton(){}
    virtual ~Singleton(){}    //virtual 

private:
    static T* instance;
};

template <typename T>
T* Singleton<T>::instance = NULL;


#endif
