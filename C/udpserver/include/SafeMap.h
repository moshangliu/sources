#ifndef SAFE_MAP_H
#define SAFE_MAP_H

#include <map>
#include <pthread.h>

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
        pthread_mutex_lock(&_mutex);
        VType& value = map[key];
        pthread_mutex_unlock(&_mutex);

        return value;
    }

    bool has(const KType& key) {
        pthread_mutex_lock(&_mutex);
        bool has = map.find(key) != map.end();
        pthread_mutex_unlock(&_mutex);

        return has;
    }

    void put(const KType& key, const VType& value) {
        pthread_mutex_lock(&_mutex);
        map[key] = value;
        pthread_mutex_unlock(&_mutex);
    }

    void erase(const KType& key) {
        pthread_mutex_lock(&_mutex);
        map.erase(key);
        pthread_mutex_unlock(&_mutex);
    }

    size_t size() {
        pthread_mutex_lock(&_mutex);
        size_t size = map.size();
        pthread_mutex_unlock(&_mutex);
        return size;
    }
};

#endif
