#include "Thread.h"

void* Thread::Func(void* obj)
{
    Thread* th = (Thread*)obj;
    return th->process();
}

int Thread::run()
{
    int ret = pthread_create(&_threadId, NULL, Thread::Func, this);
    return ret;
}

int Thread::join()
{
    int ret = pthread_join(_threadId, NULL);
    return ret;
}
