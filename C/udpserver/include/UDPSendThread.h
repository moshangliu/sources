/*
 * UDPSendThread.h
 *
 *  Created on: 2015Äê3ÔÂ9ÈÕ
 *      Author: lrt
 */

#ifndef UDPSENDTHREAD_H_
#define UDPSENDTHREAD_H_

#include "Common.h"
#include "Thread.h"
#include "UDPSendDataStructs.h"

class ClientConn;

class UDPSendThread : public Thread
{
    public:

        UDPSendThread(int listenFd, UDPSendQueue* sendQueue,
            UDPResendQueue* resendQueue, UDPAckMap* ackMap);

        virtual void* process();
        void stop() {
            _stopFlag = true;
        }

    private:
        int _listenfd;

        UDPSendQueue* _sendQueue;
        UDPResendQueue* _resendQueue;
        UDPAckMap* _ackMap;

        bool _stopFlag;
};



#endif /* UDPSENDTHREAD_H_ */
