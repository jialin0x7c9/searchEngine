#ifndef __WORDQUERYSERVER_H__
#define __WORDQUERYSERVER_H__
#include "Configuration.h"
#include "Threadpool.h"
#include "TcpServer.h"
#include "WordQuery.h"
class WordQueryServer
{
public:
    WordQueryServer(Configuration &conf);
    void start();

private:
    void onConnection(const shared_ptr<wd::TcpConnection> & conn);
    void onMessage(const shared_ptr<wd::TcpConnection> & conn);
    void onClose(const shared_ptr<wd::TcpConnection> & conn);
    void doTaskThread(const shared_ptr<wd::TcpConnection> & conn, const string &msg);

private:
    Configuration &_conf;
    WordQuery _wordQuery;
    wd::TcpServer _tcpServer;
    wd::Threadpool _threadpool;

};
#endif
