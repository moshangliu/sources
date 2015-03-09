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

class ClientConn;

class UDPSendThread : public Thread
{
    public:

        UDPSendThread(int listenFd);

        virtual void* process();

    private:
        int _listenfd;

};



#endif /* UDPSENDTHREAD_H_ */
