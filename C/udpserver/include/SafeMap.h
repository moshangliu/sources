#ifndef SAFE_MAP_H
#define SAFE_MAP_H

#include <map>
#include <pthread.h>
#include "MutexLock.h"

template <class KType, class VType>
class SafeMap {
private:
    pthread_mutex_t _mutex;
    std::map<KType, VType> map;

public:
    SafeMap() {
        _mutex = PTHREAD_MUTEX_INITIALIZER;
    }

    VType& get(const KType& key) {
        MutexLock lock(&_mutex);
        return map[key];
    }

    bool has(const KType& key) {
        MutexLock lock(&_mutex);
        return map.find(key) != map.end();
    }

    void put(const KType& key, const VType& value) {
        MutexLock lock(&_mutex);
        map[key] = value;
    }

    void erase(const KType& key) {
        MutexLock lock(&_mutex);
        map.erase(key);
    }

    size_t size() {
        MutexLock lock(&_mutex);
        return map.size();
    }
};

#endif
