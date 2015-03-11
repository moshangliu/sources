#ifndef UDPRESENDTHREAD_H_
#define UDPRESENDTHREAD_H_

#include "Common.h"
#include "Thread.h"
#include "UDPSendDataStructs.h"
#include "UDPRecvDataStructs.h"
#include "UDPPacketSendFailureHandler.h"

class ClientConn;

class UDPResendThread : public Thread
{
    public:
        UDPResendThread(int listenFd, UDPResendQueue* resendQueue,
            UDPAckMap* ackMap, UDPPacketMap* packetMap,
            UDPPacketSendFailureHandler* failureHandler);

        virtual void* process();

        void stop() {
            _stopFlag = true;
        }

        void setFailureHandler(UDPPacketSendFailureHandler* failureHandler) {
            _failureHandler = failureHandler;
        }

    private:
        int _listenfd;

        UDPResendQueue* _resendQueue;
        UDPAckMap* _ackMap;
        UDPPacketMap* _packetMap;
        UDPPacketSendFailureHandler* _failureHandler;

        bool _stopFlag;
};

#endif /* UDPRESENDTHREAD_H_ */
