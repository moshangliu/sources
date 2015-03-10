#include "Common.h"
#include "UDPResendThread.h"
#include "UDPSendDataStructs.h"
#include "UDPRetryTimeSpan.h"

#include <cstring>
#include <time.h>
#include <unistd.h>

using namespace std;

UDPResendThread::UDPResendThread(int listenFd) : _listenfd(listenFd) {}

void* UDPResendThread::process() {
    while (true) {
        UDPResendObj* obj = UDPResendQueue::instance()->top();

        int packetId = obj->frame()->packetId();
        byte frameIndex = obj->frame()->frameIndex();

        /**
         * If acked or reach max try count, remove data
         */
        if (UDPAckMap::instance()->isAcked(packetId, frameIndex)
            || obj->triedCnt() == UDPRetryTimeSpan::instance()->maxTryCnt()) {
            UDPAckMap::instance()->erase(packetId, frameIndex);
            UDPResendQueue::instance()->pop();
            delete obj;

            continue;
        }

        long currentUs = current_us();
        long runTsUs = obj->sendTsUs();
        if (currentUs >= runTsUs) {
            string ip = obj->ip();
            int port = obj->port();

            // Set remote IP / PORT
            sockaddr_in addr;
            socklen_t addrLen = sizeof(addr);
            memset(&addr, 0, addrLen);

            addr.sin_family = AF_INET;
            addr.sin_addr.s_addr = inet_addr(ip.c_str());
            addr.sin_port = htons(port);

            // Send data
            sendto(_listenfd, obj->frame()->content(), obj->frame()->contentLength(),
                0,
                (struct sockaddr*)&addr, addrLen);

            obj->update();
            UDPResendQueue::instance()->push(obj);

            continue;
        }

        usleep(runTsUs - currentUs);
    }
    return NULL;
}
