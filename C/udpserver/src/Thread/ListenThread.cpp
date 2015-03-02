#include <iostream>
#include <cstring>
#include <cstdlib>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>

#include "ClientConn.h"
#include "ListenThread.h"
#include "SafeQueueInstance.h"

using namespace std;

typedef struct sockaddr sockaddr_t;

ListenThread::ListenThread(uint32 port)
    : _port(port)
{ }

struct sockaddr_in* ListenThread::GetSvrAddr()
{
    struct sockaddr_in* svrAddr = new sockaddr_in();
    memset(svrAddr, 0, sizeof(struct sockaddr_in));
    svrAddr->sin_family = AF_INET;
    svrAddr->sin_addr.s_addr = htonl(INADDR_ANY);
    svrAddr->sin_port = htons(_port);

    return svrAddr;
}

void ListenThread::Bind(int listenfd)
{
    struct sockaddr_in* svrAddr = GetSvrAddr();
    if (bind(listenfd, (struct sockaddr*)svrAddr, sizeof(struct sockaddr_in)) < 0)
    {
        cerr << "Failed to bind port [" << _port << "]" << endl;
        exit(3);
    }
}

void ListenThread::Listen(int listenfd)
{
    if (listen(listenfd, 5) < 0)
    {
        cerr << "Failed to listen port [" << _port << "]" << endl;
        exit(4);
    }
}

void ListenThread::Accept(int listenfd)
{
    while (true)
    {
        sockaddr_t* clientAddr = new sockaddr_t();
        memset(clientAddr, 0, sizeof(sockaddr_t));

        uint32 addrLen = sizeof(sockaddr_t);
        int connfd = 0;
        if ((connfd = accept(listenfd, clientAddr, &addrLen)) < 0)
        {
            cerr << "Failed to accept a client connection. errno = [" << errno << "]" << endl;
            delete clientAddr;

            continue;
        }

        ClientConn* conn = new ClientConn(connfd,
            GetIP((struct sockaddr_in*)clientAddr),
            GetPort((struct sockaddr_in*)clientAddr));
        SafeQueueInstance::Instance()->Push(conn);

        delete clientAddr;
    }
}

void* ListenThread::Process()
{
    if (_port < 1024 && _port != 80)
    {
        cerr << "You should bind port > 1024 or 80" << endl;
        exit(1);
    }
    int listenfd = 0;
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        cerr << "Failed to create listen socket." << endl;
        exit(2);
    }

    Bind(listenfd);
    Listen(listenfd);
    Accept(listenfd);

    return NULL;
}
