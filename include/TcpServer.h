#ifndef __WD_TCPSERVER_H__
#define __WD_TCPSERVER_H__
#include "Acceptor.h"
#include "EventLoop.h"
#include "TcpConnection.h"

namespace wd
{
using TcpConnectionCallback = std::function<void(const shared_ptr<TcpConnection> &)>;

class TcpServer
{
public:
    TcpServer() = default;
    TcpServer(const string & ip, unsigned short port);
    void start();

    void setConnectionCallback(TcpConnectionCallback && cb);
    void setMessageCallback(TcpConnectionCallback && cb);
    void setCloseCallback(TcpConnectionCallback && cb);
private:
    Acceptor _acceptor;
    EventLoop _loop;
};
}
#endif
