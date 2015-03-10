
#ifndef UDPEXPIRETHREAD_H_
#define UDPEXPIRETHREAD_H_

#include "Common.h"
#include "Thread.h"

class UDPExpireThread : public Thread
{
    public:
        UDPExpireThread();
        virtual void* process();
};



#endif /* UDPEXPIRETHREAD_H_ */
