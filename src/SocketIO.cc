#include "SocketIO.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <iostream>
#include <string.h>
namespace wd
{
SocketIO::SocketIO(int fd):_fd(fd)
    {

    }

int recvCycle(int clientSocketFd, char *train, int recvLen)
{
    int totalRecv = 0;
    int ret;
    while (totalRecv < recvLen)
    {
        ret = recv(clientSocketFd, train+totalRecv, recvLen-totalRecv, 0);
        if (-1 == ret)
        {
            return -1;
        }
        totalRecv += ret;
    }
    return 0;
}

string SocketIO::read()
{
    train sendTrain;
    memset(&sendTrain, 0, sizeof(train));
    recvCycle(_fd, (char*)&sendTrain.dataLen, 4);
    recvCycle(_fd, sendTrain.data, sendTrain.dataLen);
    return string(sendTrain.data);
}


int sendCycle(int clientSocketFd, char *train, int sendLen)
{
    int totalSend = 0;
    int ret;
    while (totalSend < sendLen)
    {
        ret = send(clientSocketFd, train+totalSend, sendLen-totalSend, 0);
        if (-1 == ret)
        {
            return -1;
        }
        totalSend += ret;
    }
    return 0;
}

int SocketIO::writen(const char * buff, int len)
{
    std::cout << buff << std::endl;
    train sendTrain;
    memset(&sendTrain, 0, sizeof(train));
    /* sendTrain.dataLen = len; */
    /* std::cout << "len= " << len << std::endl; */
    /* memcpy(sendTrain.data, buff, sendTrain.dataLen); */
    /* int ret = sendCycle(_fd, (char*)&sendTrain, 4+sendTrain.dataLen); */

    off_t ptrPos = 0;
    while (ptrPos + 1024 < len)
    {
        memcpy(sendTrain.data, buff+ptrPos, 1024);
        sendTrain.dataLen = 1024;
        sendCycle(_fd, (char*)&sendTrain, 4+sendTrain.dataLen);
        ptrPos += 1024;
    }
    sendTrain.dataLen = len - ptrPos;
    memcpy(sendTrain.data, buff+ptrPos, sendTrain.dataLen);
    sendCycle(_fd, (char*)&sendTrain, 4+sendTrain.dataLen);
    return 0;
    /* int left = len; */
    /* const char * p = buff; */
    /* while (left > 0) */
    /* { */
    /*     int ret = ::write(_fd, p, left); */
    /*     if (-1 == ret && errno == EINTR) */
    /*     { */
    /*         continue; */
    /*     } */
    /*     else if (-1 == ret) */
    /*     { */
    /*         perror("write"); */
    /*         return len - left; */
    /*     } */
    /*     else */
    /*     { */
    /*         left -= ret; */
    /*         p += ret; */
    /*     } */
    /* } */
}

bool SocketIO::isClosed()
{
    int nret = 0;
    char buff[1024];
    ::memset(buff, 0, sizeof(buff));
    do
    {
        nret = ::recv(_fd, buff, sizeof(buff), MSG_PEEK);
    }while (nret == -1 && errno == EINTR);
    return (nret == 0);
}

}

