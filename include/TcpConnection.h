#ifndef __WD_TCPCONNECTION_H__
#define __WD_TCPCONNECTION_H__
#include "Noncopyable.h"
#include "Socket.h"
#include "InetAddress.h"
#include "SocketIO.h"
#include <string>
#include <memory>
#include <functional>

using std::string;

namespace wd
{

class EventLoop;

class TcpConnection : Noncopyable, public std::enable_shared_from_this<TcpConnection>
{
public:
    using TcpConnectionCallback = std::function<void(const std::shared_ptr<TcpConnection>&)>;

    //利用服务器端的accept返回的fd创建一个TcpConnection对象；
    TcpConnection(int fd, EventLoop *);
    ~TcpConnection();
    string receive();
    void send(const string & msg);
    //交由服务器的IO线程来发送；
    void sendInLoop(const string & msg);

    //打印连接以后的信息；
    string toString() const;
    void shutdown();
    bool isClosed();
public:
    void setConnectionCallback(const TcpConnectionCallback & cb)
    {
        _onConnectionCallback = std::move(cb);
    }

    void setMessageCallback(const TcpConnectionCallback & cb)
    {
        _onMessageCallback = std::move(cb);
    }

    void setCloseCallback(const TcpConnectionCallback & cb)
    {
        _onCloseCallback = std::move(cb);
    }

    void handleConnectionCallback();
    void handleMessageCallback();
    void handleCloseCallback();

private:
    InetAddress getLocalAddr(int fd);
    InetAddress getPeerAddr(int fd);
private:
    //这个_sock是客户端的socket
    Socket _sock;
    SocketIO _socketIo;
    InetAddress _localAddr;
    InetAddress _peerAddr;
    bool _isShutdwonWrite;

    EventLoop *_loop;

private:
    TcpConnectionCallback _onConnectionCallback;
    TcpConnectionCallback _onMessageCallback;
    TcpConnectionCallback _onCloseCallback;

};
}

#endif
