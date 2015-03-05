#ifndef UDP_PACKET_ID_CREATOR_H
#define UDP_PACKET_ID_CREATOR_H

#include <atomic>
#include <pthread.h>

class UDPPacketIdCreator {
private:
    std::atomic_int _creator;
    static pthread_mutex_t _mutex;
    static UDPPacketIdCreator* _instance;

    UDPPacketIdCreator() : _creator(0) { }

public:
    static UDPPacketIdCreator* instance();
    int next();
};

#endif
