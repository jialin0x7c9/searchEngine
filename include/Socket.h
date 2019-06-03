#ifndef __WD_SOCKET_H__
#define __WD_SOCKET_H__



namespace wd
{
class Socket
{
public:
    Socket();
    explicit
        Socket(int fd);
    int fd() const;
    //可以关闭写端；
    void shutdownWrite();
    ~Socket();
private:
    int _fd;
};
}
#endif
