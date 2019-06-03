#include "Threadpool.h"
#include "Thread.h"
#include "Configuration.h"
#include <unistd.h>
#include <iostream>
#include <sys/timerfd.h>
#include <poll.h>
using std::cout;
using std::endl;

namespace wd
{
Threadpool::Threadpool(size_t threadNum, size_t queSize):
    _threadNum(threadNum),
    _queSize(queSize),
    _que(_queSize),
    _isExit(false)
    {
        _threads.reserve(_threadNum);
    }

void Threadpool::start()
{
    for(size_t idx = 0; idx != _threadNum; ++idx)
    {
        //创建的每一个线程对象都放到线程池对象的vector中;
        unique_ptr<Thread> thread(new Thread(std::bind(&Threadpool::threadFunc, this)));
        _threads.push_back(std::move(thread));
    }

    for (auto it = _threads.begin(); it != _threads.end(); ++it)
    {
            (*it)->start();
    }

}

void Threadpool::stop()
{
    if(!_isExit)
    {
        //先将任务队列中的任务执行完毕
        while(!_que.empty())
        {
            ::sleep(1);
        }
        _isExit = true;
        cout << ">>> _isExit = true " << endl;
        _que.wakeup();
        for(auto & thread : _threads)
        {
            thread->join();
        }
    }
}

void Threadpool::addTask(Task && task)
{
    _que.push(task);
}

Task Threadpool::getTask()
{
    return _que.pop();
}

//每一个子线程运行
void Threadpool::threadFunc()
{
    while(!_isExit)
    {
        Task task = getTask();
        if(task)
        {
            task();
        }
    }
}

void Threadpool::handleRead(int fd)
{
    uint64_t howmany;
    int ret = ::read(fd, &howmany, sizeof(howmany));
    if(ret != sizeof(howmany))
    {
        perror("read");
    }
}

}
