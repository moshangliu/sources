#include <unistd.h>

#include "Common.h"
#include "UDPRecvDataStructs.h"
#include "UDPExpireThread.h"
#include "LoggerWrapper.h"

UDPExpireThread::UDPExpireThread(UDPRecvContainer* recvContainer)
    : _recvContainer(recvContainer), stopFlag(false) {

//    _recvContainer->print("UDPExpireThread 11");
}

void* UDPExpireThread::process() {
    LoggerWrapper::instance()->info("UDPExpireThread started");

//    _recvContainer->print("UDPExpireThread 17");
    while (!stopFlag) {
        int sleepUs = _recvContainer->expireOrReturnSleepUs();
        if (sleepUs == 0) {
            continue;
        }

        usleep(sleepUs);
    }

    LoggerWrapper::instance()->info("UDPExpireThread stopped");
    return NULL;
}

void UDPExpireThread::stop() {
    stopFlag = true;
}
