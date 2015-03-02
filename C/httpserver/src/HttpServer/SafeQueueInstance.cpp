#include "SafeQueueInstance.h"
#include "ConfManager.h"

SafeQueue<ClientConn*>* SafeQueueInstance::_sockfdQueue = NULL;
pthread_mutex_t SafeQueueInstance::_mutex = PTHREAD_MUTEX_INITIALIZER;


SafeQueue<ClientConn*>* SafeQueueInstance::Instance()
{
    if (_sockfdQueue == NULL)
    {
        pthread_mutex_lock(&_mutex);
        if (_sockfdQueue == NULL)
        {
            _sockfdQueue = new SafeQueue<ClientConn*>(ConfManager::Instance()->GetMaxConnCnt());
        }
        pthread_mutex_unlock(&_mutex);
    }
    return _sockfdQueue;
}
