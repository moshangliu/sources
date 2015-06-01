#ifndef THREAD_
#define THREAD_

#include <pthread.h>

class Thread
{
    public:
        Thread()
        {
            threadId = 0;
        }

        static void* Func(void* param);

        virtual void* Process() = 0;

        virtual ~Thread() { }

        /**
         * @brief Thread.Run()
         * @return: success 0, errno otherwise
         **/
        int Run();

        /**
         * @brief Thread.Join()
         * @return: success 0, errno otherwise
         **/
        int Join();

        pthread_t GetThreadId() { return threadId; }

    private:
        pthread_t threadId;
};


#endif
