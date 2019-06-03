#include "EventLoop.h"
#include "Acceptor.h"
#include "TcpConnection.h"

#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <sys/eventfd.h>

#include <iostream>
using std::cout;
using std::endl;

namespace wd
{
EventLoop::EventLoop(Acceptor & acceptor):
    _efd(createEpollFd()),
    _eventfd(createEventFd()),
    _acceptor(acceptor),
    _eventsList(1024),
    _isLooping(false)
    {
        //_acceptor.fd()返回的是服务器的socketfd
        //立马监听服务器的socketfd看是否有新的连接；
        addEpollReadFd(_acceptor.fd());

        addEpollReadFd(_eventfd);
    }

int EventLoop::createEpollFd()
{
    int ret = ::epoll_create1(0);
    if(ret == -1)
    {
        perror("epoll_create1");
    }
    return ret;
}

int EventLoop::createEventFd()
{
    int ret = ::eventfd(0, 0);
    if(ret == -1)
    {
        perror("eventfd");
    }
    return ret;
}

void EventLoop::addEpollReadFd(int fd)
{
    struct epoll_event evt;
    evt.data.fd = fd;
    evt.events = EPOLLIN;
    int ret = epoll_ctl(_efd, EPOLL_CTL_ADD, fd, &evt);
    if(-1 == ret)
    {
        perror("epoll_ctl");
    }
}

void EventLoop::runInLoop(Functor && cb)
{
    {
        MutexLockGuard autolock(_mutex);
        _pendingFunctors.push_back(std::move(cb));
    }

    //放入一个要执行的函数，然后通知服务器的IO线程；
    wakeup();
}


void EventLoop::wakeup()
{
    uint64_t one = 1;
    int ret = ::write(_eventfd, &one, sizeof(one));
    if(ret != sizeof(one))
    {
        perror("write");
    }
}


void EventLoop::delEpollReadFd(int fd)
{
    struct epoll_event evt;
    evt.data.fd = fd;
    int ret = epoll_ctl(_efd, EPOLL_CTL_DEL, fd, &evt);
    if(-1 == ret)
    {
        perror("epoll_ctl");
    }
}


EventLoop::~EventLoop()
{
    ::close(_efd);
}

void EventLoop::loop()
{
    _isLooping = true;
    while (_isLooping)
    {
        //即一直监听服务器的socketfd看是否有新连接
        //和
        //监听已经连接的客户端socketfd是否有发送过来数据
        waitEpollFds();
    }
}

void EventLoop::unloop()
{
    if (_isLooping)
    {
        _isLooping = false;
    }
}

void EventLoop::waitEpollFds()
{
    int nready;
    do 
    {
        //vector就相当于数组来存放每一个事件的；
        nready = ::epoll_wait(_efd, &*_eventsList.begin(), _eventsList.size(), 5000);
    }while(nready == -1 && errno == EINTR);

    if (nready == 0)
    {
    //    printf(">> epoll_wait timeout!\n");
    }
    else if (nready == -1)
    {
        ::perror(">> epoll_eait");
        return;
    }
    else
    {
        //如果监听到有消息的事件已经等于可以监听到的最大数量，则增加可以监听的数量；
        if (nready == (int)_eventsList.size())
        {
            _eventsList.resize(2 * _eventsList.size());
        }
        //nready是所有有消息的socketfd
        //这个消息要不是新连接就是客户端来消息了；
        for (int idx = 0; idx < nready; ++idx)
        {
            int fd = _eventsList[idx].data.fd;
            if (fd == _acceptor.fd() && _eventsList[idx].events & EPOLLIN)
            {
                //说明是有新的连接进来了；
                handleNewConnection();
            }
            else if (fd == _eventfd)
            {
                //说明线程池的某个线程通知IO线程要发送消息给客户端
                if(_eventsList[idx].events & EPOLLIN)
                {
                    handleRead();
                    doPendingFunctors();//在这里发送数据
                }
            }
            else
            {
                //说明是客户端发来消息了；
                handleMessage(_eventsList[idx].data.fd);
            }
        }
    }
}

void EventLoop::doPendingFunctors()
{
    //为了防止在这里做太多的开销；导致线程池不能往vector里面放发送消息的请求；
    vector<Functor> tmp;
    {
        MutexLockGuard autolock(_mutex);
        tmp.swap(_pendingFunctors);
    }

    for(auto & functor : tmp)
    {
         functor();
    }

}

void EventLoop::handleNewConnection()
{
    int peerfd = _acceptor.accept();
    //监听客户端的socketfd
    addEpollReadFd(peerfd);
    shared_ptr<TcpConnection> conn(new TcpConnection(peerfd, this));
    conn->setConnectionCallback(_onConnection);
    conn->setMessageCallback(_onMessage);
    conn->setCloseCallback(_onClose);

    _conns.insert(std::make_pair(peerfd, conn));
    
    conn->handleConnectionCallback();

}

void EventLoop::handleMessage(int fd)
{
    bool isClosed = isConnectionClosed(fd);
    auto iter = _conns.find(fd);
    //判断在map里面能不能找到
    if (iter != _conns.end())
    {
        if(!isClosed)
        {
            //iter->second是一个TcpConnection对象
            iter->second->handleMessageCallback();
        }
        else
        {
            delEpollReadFd(fd);
            iter->second->handleCloseCallback();
            _conns.erase(iter);
        }
    }
}

bool EventLoop::isConnectionClosed(int fd)
{
    int ret;
    do
    {
        char buff[1024];
        ret = recv(fd, buff, sizeof(buff), MSG_PEEK);
    }while(ret == -1 && errno == EINTR);
    return (ret == 0);
}

void EventLoop::handleRead()
{
     uint64_t howmany;
     int ret = ::read(_eventfd, &howmany, sizeof(howmany));
     if(ret != sizeof(howmany))
     {
         perror("read");
     }
}


}
