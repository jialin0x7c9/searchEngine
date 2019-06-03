#ifndef __WD_ACCEPTOR_H__
#define __WD_ACCEPTOR_H__

#include "InetAddress.h"
#include "Socket.h"

namespace wd
{
//在Acceptor对象中Bind定端口，监听和接收新连接；
//在Acceptor对象中创建InetAddress和服务器Socket对象；
class Acceptor
{
public:
    Acceptor(unsigned short port);
    Acceptor(const string & ip, unsigned short port);
    
    void ready();
    int accept();
    int fd() const;

private:
    void setReuseAddr(bool on);
    void setReusePort(bool on);
    void bind();
    void listen();
private:
    InetAddress _addr;
    Socket _listensock;
};
}


#endif
