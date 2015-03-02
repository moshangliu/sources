#ifndef CLIENTCONN_H_
#define CLIENTCONN_H_

#include "Common.h"
#include <string>

class ClientConn
{
    private:
        int _connfd;
        std::string _ip;
        uint16 _port;

    public:
        ClientConn(int connfd, std::string ip, uint32 port);
        int GetConnFd();
        std::string GetIP();
        uint16 GetPort();
};

#endif /* CLIENTCONN_H_ */
