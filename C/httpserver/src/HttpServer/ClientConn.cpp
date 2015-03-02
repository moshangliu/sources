#include "ClientConn.h"

using namespace std;

ClientConn::ClientConn(int connfd, std::string ip, uint32 port)
    : _connfd(connfd), _ip(ip), _port(port)
{ }

int ClientConn::GetConnFd()
{
    return _connfd;
}

string ClientConn::GetIP()
{
    return _ip;
}

uint16 ClientConn::GetPort()
{
    return _port;
}

