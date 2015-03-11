
#ifndef UDPEXPIRETHREAD_H_
#define UDPEXPIRETHREAD_H_

#include "Common.h"
#include "Thread.h"
#include "UDPRecvDataStructs.h"

class UDPExpireThread : public Thread
{
    public:
        UDPExpireThread(UDPRecvContainer* recvContainer);
        virtual void* process();

        void stop();

    private:
        UDPRecvContainer* _recvContainer;
        bool stopFlag;
};



#endif /* UDPEXPIRETHREAD_H_ */
