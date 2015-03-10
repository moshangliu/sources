#ifndef UDPRESENDTHREAD_H_
#define UDPRESENDTHREAD_H_

#include "Common.h"
#include "Thread.h"

class ClientConn;

class UDPResendThread : public Thread
{
    public:
        UDPResendThread(int listenFd);
        virtual void* process();

    private:
        int _listenfd;
};

#endif /* UDPRESENDTHREAD_H_ */
