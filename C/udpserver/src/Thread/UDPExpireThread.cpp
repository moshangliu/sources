#include <unistd.h>

#include "Common.h"
#include "UDPRecvDataStructs.h"
#include "UDPExpireThread.h"
#include "LoggerWrapper.h"

UDPExpireThread::UDPExpireThread() {}

void* UDPExpireThread::process() {
    LoggerWrapper::instance()->info("UDPExpireThread started");

    while (true) {
        int sleepUs = UDPRecvContainer::instance()->expireOrReturnSleepUs();
        if (sleepUs == 0) {
            continue;
        }

        usleep(sleepUs);
    }
    return NULL;
}
