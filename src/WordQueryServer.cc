#include "WordQueryServer.h"
#include <iostream>

using std::cout;
using std::endl;

WordQueryServer::WordQueryServer(Configuration & conf):
    _conf(conf),
    _wordQuery(_conf),
    _tcpServer(_conf.getConfigMap()["serverIp"],atoi(_conf.getConfigMap()["serverPort"].c_str())),
    _threadpool(atoi(_conf.getConfigMap()["threadNum"].c_str()),atoi(_conf.getConfigMap()["queueSize"].c_str()))
{

}
void WordQueryServer::onConnection(const shared_ptr<wd::TcpConnection> & conn)
{
    cout << conn->toString() << " has connected!" << endl;
}

void WordQueryServer::onMessage(const shared_ptr<wd::TcpConnection> & conn)
{
    /* cout << "onMessage...." << endl; */
    string msg = conn->receive();
    cout << ">> receive msg from client: " << msg << endl;
    _threadpool.addTask(std::bind(&WordQueryServer::doTaskThread, this, conn, msg));
}

void WordQueryServer::doTaskThread(const shared_ptr<wd::TcpConnection> & conn, const string &msg)
{
    string ret = _wordQuery.doQuery(msg);
    /* cout << "Search Result :" << endl; */
    /* cout << ret << endl; */
    conn->sendInLoop(ret);
}

void WordQueryServer::onClose(const shared_ptr<wd::TcpConnection> & conn)
{
    cout << "onClose...." << endl;
    cout << conn->toString() << " has closed!" << endl;
}


void WordQueryServer::start()
{
    _tcpServer.setConnectionCallback(std::bind(&WordQueryServer::onConnection, this, std::placeholders::_1));
    _tcpServer.setMessageCallback(std::bind(&WordQueryServer::onMessage, this, std::placeholders::_1));
    _tcpServer.setCloseCallback(std::bind(&WordQueryServer::onClose, this, std::placeholders::_1));
    _threadpool.start();
    _tcpServer.start();
}
