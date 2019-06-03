#ifndef __WD_THREAD_H__
#define __WD_THREAD_H__
#include "Noncopyable.h"
#include <pthread.h>
#include <functional>
#include <map>


using std::map;
namespace wd
{
class Thread : Noncopyable
    {
    public:
        using ThreadCallback = std::function<void()>;
        Thread(ThreadCallback && cb);
        void start();
        void join();
        int getPthid()const;
        ~Thread();
    private:
        static void * threadFunc(void *);
    private:
        pthread_t _pthid;
        ThreadCallback _cb;
        bool _isRunning;
    };
}


#endif
