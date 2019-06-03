#ifndef __WD_EVENTLOOP_H__
#define __WD_EVENTLOOP_H__
#include "MutexLock.h"
#include <sys/epoll.h>
#include <vector>
#include <map>
#include <memory>
#include <functional>
using std::vector;
using std::map;
using std::shared_ptr;

namespace wd
{
class Acceptor;
class TcpConnection;
class EventLoop
{
public:
    using TcpConnectionCallback = std::function<void(const shared_ptr<TcpConnection>&)>;
    using Functor = std::function<void()>;
    //Acceptor保存了服务器端的socketfd
    EventLoop(Acceptor &acceptor);
    ~EventLoop();

    void loop();
    void unloop();
    void runInLoop(Functor && cb);

public:
    void setConnectionCallback(TcpConnectionCallback && cb)
    {
         _onConnection = std::move(cb);
    }
    void setMessageCallback(TcpConnectionCallback && cb)
    {
        _onMessage = std::move(cb);
    }
    void setCloseCallback(TcpConnectionCallback && cb)
    {
        _onClose = std::move(cb);
    }


private:
    int createEpollFd();
    int createEventFd();

    //添加一个监听
    void addEpollReadFd(int fd);
    void delEpollReadFd(int fd);

    void waitEpollFds();
    void handleNewConnection();
    void handleMessage(int peerfd);
    bool isConnectionClosed(int fd);

    //当要服务器线程发送消息的时候子线程会write通知服务器线程
    //write完以后服务器线程就要处理掉write过来的消息；
    void handleRead();
    //提供给线程池内的每一个TcpConnection对象调用
    //当调用这个的时候代表要让服务器要让IO线程发送消息给客户端；
    void wakeup();
    //IO线程对线程池中需要发送的线程做发送消息给客户端的动作；
    void doPendingFunctors();

private:
    
    //这个是epoll的fd；
    int _efd;
    int _eventfd;//线程间通信用的fd
    Acceptor & _acceptor;
    
    //客户端socketdfd---客户端连接的对象；
    map<int, shared_ptr<TcpConnection>> _conns;

    vector<struct epoll_event> _eventsList;
    bool _isLooping;
private:
    TcpConnectionCallback _onConnection;
    TcpConnectionCallback _onMessage;
    TcpConnectionCallback _onClose;

private:
    MutexLock _mutex;
    vector<Functor> _pendingFunctors;
};




















}

#endif
