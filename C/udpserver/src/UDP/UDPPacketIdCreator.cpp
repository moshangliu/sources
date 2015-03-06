#include "UDPPacketIdCreator.h"
#include "MutexLock.h"

using namespace std;

UDPPacketIdCreator* UDPPacketIdCreator::_instance = NULL;
pthread_mutex_t UDPPacketIdCreator::_mutex = PTHREAD_MUTEX_INITIALIZER;

UDPPacketIdCreator* UDPPacketIdCreator::instance() {
    if (_instance == NULL) {
        MutexLock lock(&_mutex);
        if (_instance == NULL) {
            _instance = new UDPPacketIdCreator();
        }
    }

    return _instance;
}

int UDPPacketIdCreator::next() {
    return _creator++;
}
