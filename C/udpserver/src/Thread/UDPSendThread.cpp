#include "UDPSendDataStructs.h"
#include "UDPSendThread.h"
#include "LoggerWrapper.h"

#include <string>
#include <cstring>
#include <arpa/inet.h>

using namespace std;

UDPSendThread::UDPSendThread(int listenFd) : _listenfd(listenFd) {}

void* UDPSendThread::process() {
    while (true) {
        UDPResendObj* obj = UDPSendQueue::instance()->pop();
        string ip = obj->ip();
        int port = obj->port();
        int packetId = obj->frame()->packetId();
        byte frameCount = obj->frame()->frameCount();
        byte frameIndex = obj->frame()->frameIndex();

        // Set remote IP / PORT
        sockaddr_in addr;
        socklen_t addrLen = sizeof(addr);
        memset(&addr, 0, addrLen);

        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = inet_addr(ip.c_str());
        addr.sin_port = htons(port);

        // Init state
        obj->update();

        UDPAckMap::instance()->setNotAcked(packetId, frameIndex);
        UDPResendQueue::instance()->push(obj);

        // Send data
        sendto(_listenfd, obj->frame()->content(), obj->frame()->contentLength(),
            0,
            (struct sockaddr*)&addr, addrLen);

        LoggerWrapper::instance()->debug("UDPTrace, SEND_PACKET, %s:%d, PACKET_ID:%d, %d/%d",
            ip.c_str(), port, packetId, frameIndex, frameCount);
    }
    return NULL;
}
