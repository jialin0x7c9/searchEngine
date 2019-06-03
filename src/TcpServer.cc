#include "TcpServer.h"

namespace wd
{
TcpServer::TcpServer(const string & ip, unsigned short port):_acceptor(ip, port),_loop(_acceptor)
    {

    }

void TcpServer::start()
{
    //_acceptor完成bind和监听端口；
    _acceptor.ready();
    //_loop负责接待新连接和新消息
    _loop.loop();
}

void TcpServer::setConnectionCallback(TcpConnectionCallback && cb)
{
    _loop.setConnectionCallback(std::move(cb));
}

void TcpServer::setMessageCallback(TcpConnectionCallback && cb)
{
    _loop.setMessageCallback(std::move(cb));
}

void TcpServer::setCloseCallback(TcpConnectionCallback && cb)
{
    _loop.setCloseCallback(std::move(cb));
}




}
