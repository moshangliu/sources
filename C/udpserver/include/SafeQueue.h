#ifndef SAFETHREAD_H_
#define SAFETHREAD_H_

#include <queue>
#include <pthread.h>

#include "Common.h"

template <typename T>
class SafeQueue
{
    public:
        SafeQueue(uint32 maxSize);

        ~SafeQueue();

        void push(T obj);

        T pop();

        uint32 size();
    private:
        std::queue<T> _container;
        uint32 _maxSize;

        pthread_mutex_t _mutex;
        pthread_cond_t _not_empty_cond;
        pthread_cond_t _not_full_cond;
};

template <typename T> SafeQueue<T>::SafeQueue(uint32 maxSize)
{
    _maxSize = maxSize;
    pthread_mutex_init(&_mutex, NULL);
    pthread_cond_init(&_not_empty_cond, NULL);
    pthread_cond_init(&_not_full_cond, NULL);
}

template <typename T> SafeQueue<T>::~SafeQueue()
{
    //TODO: Will destory _mutex / _not_empty_cond / _not_full_cond
}

template <typename T> void SafeQueue<T>::push(T obj)
{
    pthread_mutex_lock(&_mutex);

    while (_container.size() == _maxSize)
    {
        pthread_cond_wait(&_not_full_cond, &_mutex);
    }

    _container.push(obj);
    pthread_cond_signal(&_not_empty_cond);

    pthread_mutex_unlock(&_mutex);
}

template <typename T> T SafeQueue<T>::pop()
{
    pthread_mutex_lock(&_mutex);

    while (_container.size() == 0)
    {
        pthread_cond_wait(&_not_empty_cond, &_mutex);
    }

    T res = _container.front();
    _container.pop();
    pthread_cond_signal(&_not_full_cond);

    pthread_mutex_unlock(&_mutex);

    return res;
}

template <typename T> uint32 SafeQueue<T>::size()
{
    pthread_mutex_lock(&_mutex);
    uint32 t_size = _container.size();
    pthread_mutex_unlock(&_mutex);

    return t_size;
}

#endif
