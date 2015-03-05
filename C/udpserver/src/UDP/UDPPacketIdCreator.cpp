#include "UDPPacketIdCreator.h"

using namespace std;

UDPPacketIdCreator* UDPPacketIdCreator::_instance = NULL;
pthread_mutex_t UDPPacketIdCreator::_mutex = PTHREAD_MUTEX_INITIALIZER;

UDPPacketIdCreator* UDPPacketIdCreator::instance() {
    if (_instance == NULL) {
        pthread_mutex_lock(&_mutex);
        if (_instance == NULL) {
            _instance = new UDPPacketIdCreator();
        }
        pthread_mutex_unlock(&_mutex);
    }

    return _instance;
}

int UDPPacketIdCreator::next() {
    _creator += 1;
    int next = _creator;
    return next;
}
