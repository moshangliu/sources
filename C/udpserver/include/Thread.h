#ifndef THREAD_
#define THREAD_

#include <pthread.h>

class Thread
{
    public:
        Thread()
        {
            _threadId = 0;
        }

        static void* Func(void* param);

        virtual void* process() = 0;

        virtual ~Thread() { }

        /**
         * @brief Thread.Run()
         * @return: success 0, errno otherwise
         **/
        int run();

        /**
         * @brief Thread.Join()
         * @return: success 0, errno otherwise
         **/
        int join();

        pthread_t threadId() { return _threadId; }

    private:
        pthread_t _threadId;
};


#endif
