#include <iostream>
#include <cstring>
#include <cstdlib>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>

#include "ListenThread.h"

using namespace std;

typedef struct sockaddr sockaddr_t;

ListenThread::ListenThread(uint32 port) : _port(port) {
    _listenfd = 0;
    if ((_listenfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        cerr << "Failed to create listen socket." << endl;
        exit(2);
    }
}

struct sockaddr_in* ListenThread::getSvrAddr() {
    struct sockaddr_in* svrAddr = new sockaddr_in();
    memset(svrAddr, 0, sizeof(struct sockaddr_in));
    svrAddr->sin_family = AF_INET;
    svrAddr->sin_addr.s_addr = htonl(INADDR_ANY);
    svrAddr->sin_port = htons(_port);

    return svrAddr;
}

void ListenThread::Bind(int listenfd) {
    struct sockaddr_in* svrAddr = getSvrAddr();
    if (bind(listenfd, (struct sockaddr*)svrAddr, sizeof(struct sockaddr_in)) < 0){
        cerr << "Failed to bind port [" << _port << "]" << endl;
        exit(3);
    }
}

void ListenThread::accept(int listenfd) {
    while (true) {
        const int BUF_LEN = 2048;
        char* buf = new char[BUF_LEN];
        memset(buf, 0, BUF_LEN);

        struct sockaddr_in clientAddr;
        socklen_t addrLen = sizeof(clientAddr);
        int n = recvfrom(listenfd, buf, BUF_LEN, 0, (struct sockaddr*)&clientAddr, &addrLen);
        if (n <= 0) {
            cerr << "Failed to recv data";
            delete [] buf;
            continue;
        }

        string ip = string(inet_ntoa(clientAddr.sin_addr));
        short port = ntohs(clientAddr.sin_port);
        cout << "Recv, From:" << ip << "-" << port
             << ", Data:" << string(buf) << endl;
        sendto(listenfd, buf, n, 0, (struct sockaddr*)&clientAddr, sizeof(sockaddr_in));
        delete [] buf;
    }
}

void* ListenThread::process() {
    if (_port < 0) {
        cerr << "You should bind port > 0" << endl;
        exit(1);
    }

    Bind(_listenfd);
    accept(_listenfd);

    return NULL;
}
