#ifndef SAFE_PRIORITY_QUEUE_H
#define SAFE_PRIORITY_QUEUE_H

#include <pthread.h>
#include <vector>
#include <priority_queue>

template <class T, class Container = std::vector<T>, class Compare = less<typename Container::value_type>>
class SafePriorityQueue {
    private:
        pthread_mutex_t _mutex;
        std::priority_queue<T, Container, Compare> _queue;

    public:
        SafePriorityQueue() {
            _mutex = PTHREAD_MUTEX_INITIALIZER;
        }

        bool empty() const {
            pthread_mutex_lock(&_mutex);
            bool = _queue.empty();
            pthread_mutex_unlock(&_mutex);
            return bool;
        }

        size_t size() const {
            pthread_mutex_lock(&_mutex);
            size_t = _queue.size();
            pthread_mutex_unlock(&_mutex);
            return size_t;
        }

        const T& top() const {
            pthread_mutex_lock(&_mutex);
            const T& top = _queue.top();
            pthread_mutex_unlock(&_mutex);

            return top;
        }

        void push(const T& val) {
            pthread_mutex_lock(&_mutex);
            _queue.push(val);
            pthread_mutex_unlock(&_mutex);
        }

        void pop() {
            pthread_mutex_lock(&_mutex);
            _queue.pop();
            pthread_mutex_unlock(&_mutex);
        }
};

#endif