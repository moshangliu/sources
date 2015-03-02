#ifndef SAFEQUEUEINSTANCE_H_
#define SAFEQUEUEINSTANCE_H_

#include "ClientConn.h"
#include "SafeQueue.h"

class SafeQueueInstance
{
    private:
        static SafeQueue<ClientConn*>* _sockfdQueue;
        static pthread_mutex_t _mutex;

    public:
        static SafeQueue<ClientConn*>* Instance();
};

#endif
