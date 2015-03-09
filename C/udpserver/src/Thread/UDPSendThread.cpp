#include "UDPSendDataStructs.h"
#include "UDPSendThread.h"
#include <arpa/inet.h>

UDPSendThread::UDPSendThread(int listenFd) : _listenfd(listenFd) {}

void* UDPSendThread::process() {
    while (true) {
        UDPResendObj* obj = UDPSendQueue::instance()->pop();

        // Set remote IP / PORT
        sockaddr_in addr;
        socklen_t addrLen = sizeof(addr);
        memset(addr, 0, addrLen);

        svrAddr.sin_family = AF_INET;
        svrAddr.sin_addr = inet_addr(obj->ip().c_str());
        svrAddr.sin_port = htons(obj->port());

        // Send data
        sendto(_listenfd, obj->frame()->content(), obj->frame()->contentLength(),
            0,
            (struct sockaddr*)&addr, &addrLen);

        // Init state
        int packetId = (*it)->packetId();
        byte frameCount = (*it)->frameCount();
        byte frameIndex = (*it)->frameIndex();
        obj->update();
        UDPAckMap::instance()->setAcked(packetId, frameIndex, false);
        UDPResendQueue::instance()->push(obj);

        LoggerWrapper::instance()->debug("UDPTrace, SEND_PACKET, %s:%d, PACKET_ID:%d, %d/%d",
            ip.c_str(), port, packetId, frameIndex, frameCount);
    }
    return NULL;
}
