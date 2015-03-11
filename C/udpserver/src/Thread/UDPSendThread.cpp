#include "UDPSendDataStructs.h"
#include "UDPSendThread.h"
#include "LoggerWrapper.h"

#include <string>
#include <cstring>
#include <arpa/inet.h>

using namespace std;

UDPSendThread::UDPSendThread(int listenFd, UDPSendQueue* sendQueue,
    UDPResendQueue* resendQueue, UDPAckMap* ackMap)
    : _listenfd(listenFd), _sendQueue(sendQueue), _resendQueue(resendQueue), _ackMap(ackMap), _stopFlag(false) {}

void* UDPSendThread::process() {
    LoggerWrapper::instance()->info("UDPSendThread started");

    while (!_stopFlag) {
        UDPResendObj* obj = _sendQueue->pop();
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

        _ackMap->setNotAcked(packetId, frameIndex);
        _resendQueue->push(obj);

        // Send data
        sendto(_listenfd, obj->frameSerializedBin(), obj->frameSerializedLen(),
            0,
            (struct sockaddr*)&addr, addrLen);

        LoggerWrapper::instance()->debug("UDPTrace, SEND_FRAME, %s:%d, PACKET_ID:%d, %d/%d, TriedCnt:%d",
            ip.c_str(), port, packetId, frameIndex, frameCount, obj->triedCnt());
    }

    LoggerWrapper::instance()->info("UDPSendThread stopped");
    return NULL;
}
