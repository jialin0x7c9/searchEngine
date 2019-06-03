#ifndef __WD_THREADPOOL_H__
#define __WD_THREADPOOL_H__
#include "TaskQueue.h"
#include "Thread.h"
#include <map>

#include <vector>
#include <memory>
#include <functional>
using std::vector;
using std::unique_ptr;
using std::map;

namespace wd
{
class Threadpool
{
public:
    Threadpool(size_t, size_t);
    void start();
    void stop();

    void addTask(Task && task);

private:
    void threadFunc();
    Task getTask();
private:
    size_t _threadNum;
    size_t _queSize;
    vector<unique_ptr<Thread>> _threads;
    TaskQueue _que;
    bool _isExit;

private:
    void handleRead(int fd);
};
}



#endif
