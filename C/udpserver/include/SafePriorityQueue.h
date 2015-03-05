#ifndef SAFE_PRIORITY_QUEUE_H
#define SAFE_PRIORITY_QUEUE_H

#include <pthread.h>
#include <vector>
#include <queue>

#include "MutexLock.h"

template <class T, class Container = std::vector<T>, class Compare = std::less<typename Container::value_type>>
class SafePriorityQueue {
    private:
        pthread_mutex_t _mutex;

        std::priority_queue<T, Container, Compare> _queue;

    public:
        SafePriorityQueue() {
            _mutex = PTHREAD_MUTEX_INITIALIZER;
        }

        bool empty()  {
            MutexLock lock(&_mutex);
//            pthread_mutex_lock(&_mutex);
            bool empty = _queue.empty();
//            pthread_mutex_unlock(&_mutex);
            return empty;
        }

        size_t size()  {
            MutexLock lock(&_mutex);
//            pthread_mutex_lock(&_mutex);
            size_t size = _queue.size();
//            pthread_mutex_unlock(&_mutex);
            return size;
        }

        const T& top()  {
            MutexLock lock(&_mutex);
//            pthread_mutex_lock(&_mutex);
            const T& top = _queue.top();
//            pthread_mutex_unlock(&_mutex);

            return top;
        }

        void push(const T& val) {
            MutexLock lock(&_mutex);
//            pthread_mutex_lock(&_mutex);
            _queue.push(val);
//            pthread_mutex_unlock(&_mutex);
        }

        void pop() {
            MutexLock lock(&_mutex);
//            pthread_mutex_lock(&_mutex);
            _queue.pop();
//            pthread_mutex_unlock(&_mutex);
        }
};

#endif
