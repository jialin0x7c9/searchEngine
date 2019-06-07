#include "RedisCache.h"
#include <stdlib.h>

using std::endl;
using std::cout;

RedisCache::RedisCache(Configuration &conf):_conf(conf)
{
    cout << "RedisCache()" << endl;
    string host = _conf.getConfigMap()["RedisIp"];
    string port = _conf.getConfigMap()["RedisPort"];
    connect(host, atoi(port.c_str()));
}


bool RedisCache::connect(string host, int port)
{
    _connect = redisConnect(host.c_str(), port);
    if (_connect != NULL && _connect->err)
    {
        cout << "Redis connection failed" << endl;
        return false;
    }
    cout << "Redis Connection success." << endl;
    return true;
}


bool RedisCache::set(string key, string value)
{
    _reply = (redisReply*)redisCommand(_connect, "SET %s %s", key.c_str(), value.c_str());
    cout << "Redis set() successfully" << endl;
    freeReplyObject(_reply);
    return true;
}


bool RedisCache::getValue(string key, string &value)
{
    _reply = (redisReply *)redisCommand(_connect, "Get %s", key.c_str());
    if (4 == _reply->type)
    {
        cout << "GetValue null" << endl; 
        return false;
    }
    cout << "getValue success" << endl;
    value = _reply->str;
    freeReplyObject(_reply);
    return true;
}

