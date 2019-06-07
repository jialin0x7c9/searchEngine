#ifndef __REDISCACHE_H__
#define __REDISCACHE_H__
#include <hiredis/hiredis.h>
#include <string>
#include "Configuration.h"

using std::string;

class RedisCache
{
public:
    RedisCache(Configuration &conf);
    bool set(string key, string value);
    bool getValue(string key, string &value);
private:
    bool connect(string host, int port);
private:
    Configuration &_conf;
    redisContext *_connect;
    redisReply *_reply;
};

#endif
