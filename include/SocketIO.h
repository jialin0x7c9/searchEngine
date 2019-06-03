#ifndef __WD_SOCKETIO_H__
#define __WD_SOCKETIO_H__
#include <iostream>
using std::string;
typedef struct
{
    int dataLen;
    char data[1024];
}train;

namespace wd
{
class SocketIO
{
public:
    explicit
        SocketIO(int fd);
    int readn(char * buff, int len);
    std::string read();
    int readline(char * buff, int maxlen);
    int writen(const char * buff, int len);

    bool isClosed();

private:
    int recvPeek(char *buff, int len);
private:
    int _fd;
};
}
#endif
