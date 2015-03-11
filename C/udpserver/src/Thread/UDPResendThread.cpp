#include "Common.h"
#include "UDPResendThread.h"
#include "UDPSendDataStructs.h"
#include "UDPRetryTimeSpan.h"
#include "LoggerWrapper.h"

#include <cstring>
#include <time.h>
#include <unistd.h>

using namespace std;

UDPResendThread::UDPResendThread(int listenFd, UDPResendQueue* resendQueue, UDPAckMap* ackMap)
    : _listenfd(listenFd), _resendQueue(resendQueue), _ackMap(ackMap), _stopFlag(false) {}

void* UDPResendThread::process() {
    const int DEFAULT_SLEEP_US = 10;
    LoggerWrapper::instance()->info("UDPResendThread started");

    while (!_stopFlag) {
        UDPResendObj* obj = _resendQueue->pop();
        if (obj == NULL) {
            usleep(DEFAULT_SLEEP_US);
            continue;
        }

        string ip = obj->ip();
        int port = obj->port();
        int packetId = obj->frame()->packetId();
        byte frameIndex = obj->frame()->frameIndex();
        byte frameCount = obj->frame()->frameCount();

        /**
         * If acked or reach max try count, remove data
         */
        if (!_ackMap->needResend(packetId, frameIndex)
            || obj->triedCnt() == UDPRetryTimeSpan::instance()->maxTryCnt()) {
//            LoggerWrapper::instance()->warn("UDPTrace, SEND_FRAME_WARN, %s:%d, PACKET_ID:%d, %d/%d, TriedCnt:%d/%d",
//                ip.c_str(), port, packetId, frameIndex, frameCount, obj->triedCnt(), UDPRetryTimeSpan::instance()->maxTryCnt());

            _ackMap->erase(packetId, frameIndex);
            delete obj;

            continue;
        }

        long currentUs = current_us();
        long runTsUs = obj->sendTsUs();
        if (currentUs >= runTsUs) {

            // Set remote IP / PORT
            sockaddr_in addr;
            socklen_t addrLen = sizeof(addr);
            memset(&addr, 0, addrLen);

            addr.sin_family = AF_INET;
            addr.sin_addr.s_addr = inet_addr(ip.c_str());
            addr.sin_port = htons(port);

            // Send data
            sendto(_listenfd, obj->frameSerializedBin(), obj->frameSerializedLen(),
                0,
                (struct sockaddr*)&addr, addrLen);

            obj->update();
            _resendQueue->push(obj);

            LoggerWrapper::instance()->debug("UDPTrace, SEND_FRAME, %s:%d, PACKET_ID:%d, %d/%d, TriedCnt:%d/%d",
                ip.c_str(), port, packetId, frameIndex, frameCount, obj->triedCnt(), UDPRetryTimeSpan::instance()->maxTryCnt());

            continue;
        }

        usleep(runTsUs - currentUs);
    }

    LoggerWrapper::instance()->info("UDPResendThread stopped");
    return NULL;
}
