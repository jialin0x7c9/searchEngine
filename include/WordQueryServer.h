#ifndef __WORDQUERYSERVER_H__
#define __WORDQUERYSERVER_H__
#include "Configuration.h"
#include "Threadpool.h"
#include "TcpServer.h"
class WordQueryServer
{
public:
    WordQueryServer(Configuration &conf);
    void start();
    void onConnection(const shared_ptr<wd::TcpConnection> & conn);
    void onMessage(const shared_ptr<wd::TcpConnection> & conn);
    void onClose(const shared_ptr<wd::TcpConnection> & conn);

private:
    Configuration &_conf;
    wd::TcpServer _tcpServer;
    wd::Threadpool _threadpool;
};
#endif
