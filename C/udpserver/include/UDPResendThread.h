#ifndef UDPRESENDTHREAD_H_
#define UDPRESENDTHREAD_H_

#include "Common.h"
#include "Thread.h"
#include "UDPSendDataStructs.h"
#include "UDPRecvDataStructs.h"

class ClientConn;

class UDPResendThread : public Thread
{
    public:
        UDPResendThread(int listenFd, UDPResendQueue* resendQueue, UDPAckMap* ackMap);
        virtual void* process();
        void stop() {
            _stopFlag = true;
        }

    private:
        int _listenfd;

        UDPResendQueue* _resendQueue;
        UDPAckMap* _ackMap;

        bool _stopFlag;
};

#endif /* UDPRESENDTHREAD_H_ */
