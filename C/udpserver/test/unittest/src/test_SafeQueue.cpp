#include "SafeQueue.h"
#include "Thread.h"

#include <gtest/gtest.h>

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

class PushThread : public Thread
{
    public:
        void* process()
        {
            for (uint32 i = 0; i < _size; i++)
            {
                _queue->push(i);
            }
            return NULL;
        }

        PushThread(SafeQueue<int>* queue, uint32 size)
        {
            _queue = queue;
            _size = size;
        }

    private:
        SafeQueue<int>* _queue;
        uint32 _size;
};

class PopThread : public Thread
{
    public:
        void* process()
        {
            for (uint32 i = 0; i < _size; i++)
            {
                _queue->pop();
            }
            return NULL;
        }

        PopThread(SafeQueue<int>* queue, uint32 size)
        {
            _queue = queue;
            _size = size;
        }

    private:
        SafeQueue<int>* _queue;
        uint32 _size;
};

const int _maxSize = 10000;

class TEST_SafeQueue_Suite : public testing::Test
{
    protected:
        void SetUp()
        {
            _queue = new SafeQueue<int>(_maxSize);
        }

        void TearDown()
        {
            delete _queue;
        }

        static SafeQueue<int>* _queue;

};

SafeQueue<int>* TEST_SafeQueue_Suite::_queue = NULL;

TEST_F(TEST_SafeQueue_Suite, Test_Push_int32)
{
    PushThread pushThr(_queue, _maxSize);
    pushThr.run();
    pushThr.join();

    ASSERT_EQ(_queue->size(), _maxSize);
}

TEST_F(TEST_SafeQueue_Suite, Test_Push_Pop_Not_Parallel)
{
    PushThread pushThr(_queue, _maxSize);
    pushThr.run();
    pushThr.join();

    ASSERT_EQ(_queue->size(), _maxSize);

    PopThread popThr(_queue, _maxSize);
    popThr.run();
    popThr.join();

    ASSERT_EQ(_queue->size(), 0);
}

TEST_F(TEST_SafeQueue_Suite, Test_Push_Pop_Parallel)
{
    PushThread pushThr(_queue, _maxSize);
    PopThread popThr(_queue, _maxSize);

    pushThr.run();
    popThr.run();

    pushThr.join();
    popThr.join();

    ASSERT_EQ(_queue->size(), 0);
}

const int32 THREAD_CNT = 10;
TEST_F(TEST_SafeQueue_Suite, Test_Push_Pop_Parallel_Multi_Thread)
{
    PushThread* pushThreads[THREAD_CNT];
    PopThread* popThreads[THREAD_CNT];

    for (int i = 0; i < THREAD_CNT; i++)
    {
        pushThreads[i] = new PushThread(_queue, _maxSize);
        popThreads[i] = new PopThread(_queue, _maxSize);
    }

    for (int i = 0; i < THREAD_CNT; i++)
    {
        pushThreads[i]->run();
        popThreads[i]->run();
    }

    for (int i = 0; i < THREAD_CNT; i++)
    {
        pushThreads[i]->join();
        popThreads[i]->join();
    }

    ASSERT_EQ(_queue->size(), 0);
}
