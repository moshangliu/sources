#include "Thread.h"

void* Thread::Func(void* obj)
{
    Thread* th = (Thread*)obj;
    return th->Process();
}

int Thread::Run()
{
    int ret = pthread_create(&threadId, NULL, Thread::Func, this);
    return ret;
}

int Thread::Join()
{
    int ret = pthread_join(threadId, NULL);
    return ret;
}
